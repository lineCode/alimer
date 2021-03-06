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

#include "../../AlimerConfig.h"

#if ALIMER_COMPILE_D3D11
#include "D3D11GraphicsDevice.h"
#include "D3D11Swapchain.h"
#include "D3D11Framebuffer.h"
#include "D3D11Texture.h"
#include "D3D11CommandContext.h"
#include "D3D11GpuBuffer.h"
#include "D3D11Shader.h"
#include "D3D11Pipeline.h"
#include "../D3D/D3DPlatformFunctions.h"
#include "../ShaderCompiler.h"
#include "../../Core/Platform.h"
#include "../../Core/Log.h"
#include <STB/stb_image_write.h>

using namespace Microsoft::WRL;

namespace Alimer
{
#if defined(_DEBUG)
    // Check for SDK Layer support.
    inline bool SdkLayersAvailable()
    {
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
            0,
            D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
            nullptr,                    // Any feature level will do.
            0,
            D3D11_SDK_VERSION,
            nullptr,                    // No need to keep the D3D device reference.
            nullptr,                    // No need to know the feature level.
            nullptr                     // No need to keep the D3D device context reference.
        );

        return SUCCEEDED(hr);
    }
#endif

    D3D11GraphicsDevice::D3D11GraphicsDevice(bool validation)
        : GraphicsDevice(GraphicsBackend::Direct3D11, validation)
        , _functions(new D3DPlatformFunctions())
        , _cache(this)
    {
        if (!_functions->LoadFunctions(false))
        {
            ALIMER_LOGCRITICAL("D3D11 - Failed to load functions");
        }
    }

    D3D11GraphicsDevice::~D3D11GraphicsDevice()
    {
        Shutdown();
    }

    void D3D11GraphicsDevice::Shutdown()
    {
        SafeDelete(_mainSwapchain);

        // Clear pending resources.
        GraphicsDevice::Shutdown();

        _cache.Clear();

        _d3dContext.Reset();
        _d3dContext1.Reset();
        _d3dAnnotation.Reset();
        _d3dDevice1.Reset();

#ifdef _DEBUG
        {
            ComPtr<ID3D11Debug> d3dDebug;
            if (SUCCEEDED(_d3dDevice.As(&d3dDebug)))
            {
                d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
            }
        }
#endif

        _d3dDevice.Reset();
        SafeDelete(_functions);
    }

    bool D3D11GraphicsDevice::Initialize(const RenderingSettings& settings)
    {
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
        if (_validation)
        {
            if (SdkLayersAvailable())
            {
                // If the project is in a debug build, enable debugging via SDK Layers with this flag.
                creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
            }
            else
            {
                ALIMER_LOGWARN("Direct3D Debug Device is not available");
            }
        }
#endif
        // Determine DirectX hardware feature levels this app will support.
        static const D3D_FEATURE_LEVEL s_featureLevels[] =
        {
#if ALIMER_PLATFORM_UWP
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
#endif
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };

        UINT featLevelCount = 0;
        for (; featLevelCount < _countof(s_featureLevels); ++featLevelCount)
        {
            if (s_featureLevels[featLevelCount] < _d3dMinFeatureLevel)
                break;
        }

        ComPtr<IDXGIAdapter1> adapter;
        GetHardwareAdapter(adapter.GetAddressOf());

        HRESULT hr = E_FAIL;
        // Create the Direct3D 11 API device object and a corresponding context.
        if (adapter)
        {
            hr = D3D11CreateDevice(
                adapter.Get(),
                D3D_DRIVER_TYPE_UNKNOWN,
                0,
                creationFlags,
                s_featureLevels,
                featLevelCount,
                D3D11_SDK_VERSION,
                _d3dDevice.ReleaseAndGetAddressOf(),
                &_d3dFeatureLevel,
                _d3dContext.ReleaseAndGetAddressOf()
            );

            if (hr == E_INVALIDARG && featLevelCount > 1)
            {
                assert(s_featureLevels[0] == D3D_FEATURE_LEVEL_11_1);

                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(
                    adapter.Get(),
                    D3D_DRIVER_TYPE_UNKNOWN,
                    nullptr,
                    creationFlags,
                    &s_featureLevels[1],
                    featLevelCount - 1,
                    D3D11_SDK_VERSION,
                    _d3dDevice.ReleaseAndGetAddressOf(),
                    &_d3dFeatureLevel,
                    _d3dContext.ReleaseAndGetAddressOf()
                );
            }
        }
#if defined(NDEBUG)
        else
        {
            ALIMER_LOGCRITICAL("No Direct3D hardware device found");
        }
#else
        if (FAILED(hr))
        {
            // If the initialization fails, fall back to the WARP device.
            // For more information on WARP, see: 
            // http://go.microsoft.com/fwlink/?LinkId=286690
            hr = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
                0,
                creationFlags,
                s_featureLevels,
                _countof(s_featureLevels),
                D3D11_SDK_VERSION,
                _d3dDevice.ReleaseAndGetAddressOf(),
                &_d3dFeatureLevel,
                _d3dContext.ReleaseAndGetAddressOf()
            );

            if (hr == E_INVALIDARG && featLevelCount > 1)
            {
                assert(s_featureLevels[0] == D3D_FEATURE_LEVEL_11_1);

                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(
                    nullptr,
                    D3D_DRIVER_TYPE_WARP,
                    nullptr,
                    creationFlags,
                    &s_featureLevels[1],
                    featLevelCount - 1,
                    D3D11_SDK_VERSION,
                    _d3dDevice.ReleaseAndGetAddressOf(),
                    &_d3dFeatureLevel,
                    _d3dContext.ReleaseAndGetAddressOf()
                );
            }


            if (SUCCEEDED(hr))
            {
                ALIMER_LOGINFO("Direct3D Adapter - WARP");
            }
        }
#endif
        ThrowIfFailed(hr);

#ifndef NDEBUG
        ComPtr<ID3D11Debug> d3dDebug;
        if (SUCCEEDED(_d3dDevice.As(&d3dDebug)))
        {
            ComPtr<ID3D11InfoQueue> d3dInfoQueue;
            if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
            {
#ifdef _DEBUG
                d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
                d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
                D3D11_INFO_QUEUE_FILTER filter;
                memset(&filter, 0, sizeof(filter));

                D3D11_MESSAGE_SEVERITY denySeverity = D3D11_MESSAGE_SEVERITY_INFO;
                filter.DenyList.NumSeverities = 1;
                filter.DenyList.pSeverityList = &denySeverity;

                D3D11_MESSAGE_ID denyIds[] =
                {
                    D3D11_MESSAGE_ID_OMSETRENDERTARGETS_INVALIDVIEW,
                    D3D11_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL,
                    D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,
                    D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                };

                filter.DenyList.NumIDs = sizeof(denyIds) / sizeof(D3D11_MESSAGE_ID);
                filter.DenyList.pIDList = (D3D11_MESSAGE_ID*)&denyIds;
                d3dInfoQueue->PushStorageFilter(&filter);
            }
        }
#endif /* NDEBUG */

        // Obtain Direct3D 11.1 interfaces (if available)
        if (SUCCEEDED(_d3dDevice.As(&_d3dDevice1)))
        {
            (void)_d3dContext.As(&_d3dContext1);
            (void)_d3dContext.As(&_d3dAnnotation);
        }

        InitializeCaps();

        // Create Swapchain.
        _mainSwapchain = new D3D11Swapchain(this, &settings.swapchain);

        // Create default command context.
        _context = new D3D11CommandContext(this);

        return GraphicsDevice::Initialize(settings);
    }

    void D3D11GraphicsDevice::PresentImpl()
    {
        // Present the frame.
        _mainSwapchain->Present();

        // Flush immediate context.
        //_d3dContext->Flush();
    }

    Framebuffer* D3D11GraphicsDevice::GetSwapchainFramebuffer() const
    {
        return _mainSwapchain->GetCurrentFramebuffer();
    }

    void D3D11GraphicsDevice::GenerateScreenshot(const std::string& fileName)
    {
        /*HRESULT hr = S_OK;

        D3D11Texture* backBufferTexture = _swapChain->GetBackbufferTexture();
        const DXGI_FORMAT format = backBufferTexture->GetDXGIFormat();

        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.Width = backBufferTexture->GetWidth();
        textureDesc.Height = backBufferTexture->GetHeight();
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = format;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.BindFlags = 0;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        textureDesc.MiscFlags = 0;

        ID3D11Texture2D* texture;
        hr = _d3dDevice->CreateTexture2D(&textureDesc, nullptr, &texture);

        if (FAILED(hr))
        {
            ALIMER_LOGCRITICALF("D3D11 - Failed to create texture, error: %08X", static_cast<uint32_t>(hr));
        }

        // Resolve multisample texture.
        if (backBufferTexture->GetSamples() > SampleCount::Count1)
        {
            D3D11_TEXTURE2D_DESC resolveTextureDesc;
            resolveTextureDesc.Width = backBufferTexture->GetWidth();
            resolveTextureDesc.Height = backBufferTexture->GetHeight();
            resolveTextureDesc.MipLevels = 1;
            resolveTextureDesc.ArraySize = 1;
            resolveTextureDesc.Format = format;
            resolveTextureDesc.SampleDesc.Count = 1;
            resolveTextureDesc.SampleDesc.Quality = 0;
            resolveTextureDesc.Usage = D3D11_USAGE_DEFAULT;
            resolveTextureDesc.BindFlags = 0;
            resolveTextureDesc.CPUAccessFlags = 0;
            resolveTextureDesc.MiscFlags = 0;

            ID3D11Texture2D* resolveTexture;
            hr = _d3dDevice->CreateTexture2D(&resolveTextureDesc, nullptr, &resolveTexture);

            if (FAILED(hr))
            {
                texture->Release();
                ALIMER_LOGCRITICALF("D3D11 - Failed to create texture, error: %08X", static_cast<uint32_t>(hr));
            }

            _d3dImmediateContext->ResolveSubresource(resolveTexture, 0, backBufferTexture->GetResource(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
            _d3dImmediateContext->CopyResource(texture, resolveTexture);
            resolveTexture->Release();
        }
        else
        {
            _d3dImmediateContext->CopyResource(texture, backBufferTexture->GetResource());
        }

        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        hr = _d3dImmediateContext->Map(texture, 0, D3D11_MAP_READ, 0, &mappedSubresource);
        if (FAILED(hr))
        {
            texture->Release();
            ALIMER_LOGCRITICALF("D3D11 - Failed to map resource, error: %08X", static_cast<uint32_t>(hr));
        }

        if (!stbi_write_png(fileName.c_str(), textureDesc.Width, textureDesc.Height, 4, mappedSubresource.pData, static_cast<int>(mappedSubresource.RowPitch)))
        {
            _d3dImmediateContext->Unmap(texture, 0);
            texture->Release();
            ALIMER_LOGCRITICAL("D3D11 - Failed to save screenshot to file");
        }

        _d3dImmediateContext->Unmap(texture, 0);
        texture->Release();*/
    }

    bool D3D11GraphicsDevice::WaitIdle()
    {
        _d3dContext->Flush();
        return true;
    }

    void D3D11GraphicsDevice::GetHardwareAdapter(IDXGIAdapter1** ppAdapter)
    {
        *ppAdapter = nullptr;

        ComPtr<IDXGIFactory1> dxgiFactory;

        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))))
        {
            ALIMER_LOGCRITICAL("D3D11 - Failed to create DXGI factory");
        }

        ComPtr<IDXGIAdapter1> adapter;
        for (UINT adapterIndex = 0;
            DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(
                adapterIndex,
                adapter.ReleaseAndGetAddressOf());
            adapterIndex++)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                continue;
            }

            wchar_t buff[256] = {};
            swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
            OutputDebugStringW(buff);

            break;
        }

        *ppAdapter = adapter.Detach();
    }

    void D3D11GraphicsDevice::InitializeCaps()
    {
        ComPtr<IDXGIDevice1> dxgiDevice;
        ThrowIfFailed(_d3dDevice.As(&dxgiDevice));

        ComPtr<IDXGIAdapter> dxgiAdapter;
        ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        DXGI_ADAPTER_DESC desc;
        dxgiAdapter->GetDesc(&desc);
        _features.SetVendorId(desc.VendorId);
        _features.SetDeviceId(desc.DeviceId);
        //_features.SetDeviceName(desc.Description);

        switch (_d3dFeatureLevel)
        {
        case D3D_FEATURE_LEVEL_10_0:
            _shaderModelMajor = 4;
            _shaderModelMinor = 0;
            break;
        case D3D_FEATURE_LEVEL_10_1:
            _shaderModelMajor = 4;
            _shaderModelMinor = 1;
            break;
        case D3D_FEATURE_LEVEL_11_0:
            _shaderModelMajor = 5;
            _shaderModelMinor = 0;
            break;
        case D3D_FEATURE_LEVEL_11_1:
            _shaderModelMajor = 5;
            _shaderModelMinor = 0;
            break;
        case D3D_FEATURE_LEVEL_12_0:
            _shaderModelMajor = 5;
            _shaderModelMinor = 1;
            break;
        case D3D_FEATURE_LEVEL_12_1:
            _shaderModelMajor = 5;
            _shaderModelMinor = 1;
            break;
        default:
            break;
        }

        D3D11_FEATURE_DATA_THREADING threadingFeature = { 0 };
        HRESULT hr = _d3dDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingFeature, sizeof(threadingFeature));
        if (SUCCEEDED(hr)
            && threadingFeature.DriverConcurrentCreates
            && threadingFeature.DriverCommandLists)
        {
            _features.SetMultithreading(true);
        }

        _features.SetMaxColorAttachments(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
    }

    void D3D11GraphicsDevice::HandleDeviceLost()
    {
        // TODO
    }

    D3D11Cache &D3D11GraphicsDevice::GetCache()
    {
        return _cache;
    }


    GpuBuffer* D3D11GraphicsDevice::CreateBufferImpl(const BufferDescriptor* descriptor, const void* initialData)
    {
        return new D3D11Buffer(this, descriptor, initialData);
    }

    Texture* D3D11GraphicsDevice::CreateTextureImpl(const TextureDescriptor* descriptor, const ImageLevel* initialData)
    {
        return new D3D11Texture(this, descriptor, initialData, nullptr);
    }

    Framebuffer* D3D11GraphicsDevice::CreateFramebufferImpl(const FramebufferDescriptor* descriptor)
    {
        return new D3D11Framebuffer(this, descriptor);
    }

    Shader* D3D11GraphicsDevice::CreateShaderImpl(const ShaderDescriptor* descriptor)
    {
        return new D3D11Shader(this, descriptor);
    }

    Pipeline* D3D11GraphicsDevice::CreateRenderPipelineImpl(const RenderPipelineDescriptor* descriptor)
    {
        return new D3D11Pipeline(this, descriptor);
    }
}
#endif /* ALIMER_COMPILE_D3D11 */
