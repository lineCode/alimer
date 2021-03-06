//
// Copyright (c) 2018 Amer Koleci and contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "D3D11Swapchain.h"
#include "D3D11GraphicsDevice.h"
#include "D3D11Texture.h"
#include "D3D11Framebuffer.h"
#include "../D3D/D3DConvert.h"
#include "../../Core/Log.h"
using namespace Microsoft::WRL;

namespace Alimer
{
    D3D11Swapchain::D3D11Swapchain(D3D11GraphicsDevice* device, const SwapchainDescriptor* descriptor, uint32_t backBufferCount)
        : _device(device)
        , _backBufferCount(backBufferCount)
    {
        switch (descriptor->colorFormat)
        {
        case PixelFormat::BGRA8UNorm:
            _backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;

        case PixelFormat::BGRA8UNormSrgb:
            _backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            break;

        default:
            _backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        }

       _depthStencilFormat = descriptor->depthStencilFormat;

#if ALIMER_PLATFORM_UWP
        _window = static_cast<IUnknown*>(descriptor->windowHandle);
#else
        _hwnd = static_cast<HWND>(descriptor->windowHandle);
#endif

        Resize(descriptor->width, descriptor->height, true);
    }

    D3D11Swapchain::~D3D11Swapchain()
    {
        Destroy();
    }

    void D3D11Swapchain::Destroy()
    {
        _swapChain->SetFullscreenState(false, nullptr);

        _framebuffers.clear();
        _renderTarget.Reset();
        _depthStencil.Reset();
        _swapChain.Reset();
        _swapChain1.Reset();
    }

    void D3D11Swapchain::Resize(uint32_t width, uint32_t height, bool force)
    {
        if (_width == width && _height == height && !force)
        {
            return;
        }

        // Clear the previous window size specific context.
        ID3D11RenderTargetView* nullViews[] = { nullptr };
        _device->GetD3DDeviceContext()->OMSetRenderTargets(1, nullViews, nullptr);
        _renderTarget.Reset();
        _depthStencil.Reset();
        _device->GetD3DDeviceContext()->Flush();

        HRESULT hr = S_OK;

        if (_swapChain)
        {
            hr = _swapChain->ResizeBuffers(
                _backBufferCount,
                width,
                height,
                _backBufferFormat,
                _swapChainFlags
            );

            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {
            }
        }
        else
        {
            // This sequence obtains the DXGI factory that was used to create the Direct3D device above.
            ComPtr<IDXGIDevice1> dxgiDevice;
            ThrowIfFailed(_device->GetD3DDevice()->QueryInterface(dxgiDevice.ReleaseAndGetAddressOf()));

            ComPtr<IDXGIAdapter> dxgiAdapter;
            ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

            ComPtr<IDXGIFactory1> dxgiFactory;
            ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

            // Check tearing.
            ComPtr<IDXGIFactory5> factory5;
            bool allowTearing = false;
            if (SUCCEEDED(dxgiFactory.As(&factory5)))
            {
                BOOL bAllowTearing;
                hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bAllowTearing, sizeof(BOOL));
                if (FAILED(hr) || !bAllowTearing)
                {
#ifdef _DEBUG
                    ALIMER_LOGWARN("Variable refresh rate displays not supported.");
#endif
                }
                else
                {
                    allowTearing = true;
                    _syncInterval = 0;
                    _presentFlags = DXGI_PRESENT_ALLOW_TEARING;
                    _swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
                }
            }

            ComPtr<IDXGIFactory2> dxgiFactory2;
            if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
            {
                // DirectX 11.1 or later.
                DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
                swapChainDesc.Width = width;
                swapChainDesc.Height = height;
                swapChainDesc.Format = _backBufferFormat;
                swapChainDesc.SampleDesc.Count = 1;
                swapChainDesc.SampleDesc.Quality = 0;
                swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                swapChainDesc.BufferCount = _backBufferCount;
                swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
                swapChainDesc.SwapEffect = allowTearing ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD;
                swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
                swapChainDesc.Flags = _swapChainFlags;

                if (!_window)
                {
                    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
                    fsSwapChainDesc.Windowed = TRUE;

                    hr = dxgiFactory2->CreateSwapChainForHwnd(
                        _device->GetD3DDevice(),
                        _hwnd,
                        &swapChainDesc,
                        &fsSwapChainDesc,
                        nullptr,
                        _swapChain1.ReleaseAndGetAddressOf()
                    );

                    if (FAILED(hr))
                    {
                        ALIMER_LOGCRITICALF("Failed to create DXGI SwapChain, HRESULT %08X", static_cast<unsigned int>(hr));
                    }

                    ThrowIfFailed(_swapChain1.As(&_swapChain));
                    ThrowIfFailed(dxgiFactory->MakeWindowAssociation(_hwnd, DXGI_MWA_NO_ALT_ENTER));
                }
                else
                {
                    swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
                    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

                    ThrowIfFailed(dxgiFactory2->CreateSwapChainForCoreWindow(
                        _device->GetD3DDevice(),
                        _window,
                        &swapChainDesc,
                        nullptr,
                        &_swapChain1
                    ));

                    // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
                    // ensures that the application will only render after each VSync, minimizing power consumption.
                    ComPtr<IDXGIDevice3> dxgiDevice3;
                    ThrowIfFailed(_device->GetD3DDevice()->QueryInterface(IID_PPV_ARGS(&dxgiDevice3)));
                    ThrowIfFailed(dxgiDevice3->SetMaximumFrameLatency(1));

                    // TODO: Handle rotation.
                    ThrowIfFailed(_swapChain1->SetRotation(DXGI_MODE_ROTATION_IDENTITY));
                }
            }
            else
            {
                // DirectX 11.0
                DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
                swapChainDesc.BufferDesc.Width = _width;
                swapChainDesc.BufferDesc.Height = _height;
                swapChainDesc.BufferDesc.Format = _backBufferFormat;
                swapChainDesc.SampleDesc.Count = 1;
                swapChainDesc.SampleDesc.Quality = 0;
                swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                swapChainDesc.BufferCount = _backBufferCount;
                swapChainDesc.OutputWindow = _hwnd;
                swapChainDesc.Windowed = TRUE;
                swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

                ThrowIfFailed(dxgiFactory->CreateSwapChain(
                    _device->GetD3DDevice(),
                    &swapChainDesc,
                    _swapChain.ReleaseAndGetAddressOf()
                ));
            }
        }

        ID3D11Texture2D* renderTarget;
        ThrowIfFailed(_swapChain->GetBuffer(0, IID_PPV_ARGS(&renderTarget)));

        D3D11_TEXTURE2D_DESC d3dTextureDesc;
        renderTarget->GetDesc(&d3dTextureDesc);

        TextureDescriptor textureDesc = {};
        textureDesc.type = TextureType::Type2D;
        textureDesc.format = d3d::Convert(d3dTextureDesc.Format);
        textureDesc.width = d3dTextureDesc.Width;
        textureDesc.height = d3dTextureDesc.Height;
        textureDesc.depth = 1;
        if (d3dTextureDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
        {
            textureDesc.type = TextureType::TypeCube;
            textureDesc.arrayLayers = d3dTextureDesc.ArraySize / 6;
        }
        else
        {
            textureDesc.arrayLayers = d3dTextureDesc.ArraySize;
        }
        textureDesc.mipLevels = d3dTextureDesc.MipLevels;
        textureDesc.usage = TextureUsage::Unknown;
        if (d3dTextureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            textureDesc.usage |= TextureUsage::ShaderRead;
        }

        if (d3dTextureDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
        {
            textureDesc.usage |= TextureUsage::ShaderWrite;
        }

        if (d3dTextureDesc.BindFlags & D3D11_BIND_RENDER_TARGET
            || d3dTextureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
        {
            textureDesc.usage |= TextureUsage::RenderTarget;
        }

        _renderTarget = new D3D11Texture(_device, &textureDesc, nullptr, renderTarget);

        FramebufferDescriptor fboDescriptor = {};
        fboDescriptor.colorAttachments[0].texture = _renderTarget.Get();
        // Create depth stencil if required.
        if (_depthStencilFormat != PixelFormat::Unknown)
        {
            textureDesc.type = TextureType::Type2D;
            textureDesc.format = _depthStencilFormat;
            textureDesc.width = d3dTextureDesc.Width;
            textureDesc.height = d3dTextureDesc.Height;
            textureDesc.usage = TextureUsage::RenderTarget;
            _depthStencil = new D3D11Texture(_device, &textureDesc, nullptr, nullptr);
            fboDescriptor.depthStencilAttachment.texture = _depthStencil.Get();
        }

        _framebuffers.resize(1);
        _framebuffers[0] = _device->CreateFramebuffer(&fboDescriptor);

        // Set new size.
        _width = width;
        _height = height;
    }

    void D3D11Swapchain::Present()
    {
        HRESULT hr = _swapChain->Present(_syncInterval, _presentFlags);
        //m_d3dContext->DiscardView(m_d3dRenderTargetView.Get());

        // If the device was removed either by a disconnection or a driver upgrade, we 
        // must recreate all device resources.
        if (hr == DXGI_ERROR_DEVICE_REMOVED
            || hr == DXGI_ERROR_DEVICE_RESET)
        {
#ifdef _DEBUG
            char buff[64] = {};
            sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? _device->GetD3DDevice()->GetDeviceRemovedReason() : hr);
            OutputDebugStringA(buff);
#endif

            _device->HandleDeviceLost();
        }
        else
        {
            ThrowIfFailed(hr);
        }
    }

    Framebuffer* D3D11Swapchain::GetCurrentFramebuffer() const
    {
        return _framebuffers[_currentBackBufferIndex];
    }
}
