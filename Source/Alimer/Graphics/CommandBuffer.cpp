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

#include "Graphics/CommandBuffer.h"
#include "Graphics/Graphics.h"

namespace Alimer
{
	CommandBuffer::CommandBuffer(Graphics* graphics)
		: GpuResource(graphics, GpuResourceType::CommandBuffer)
	{
		ResetState();
	}

	CommandBuffer::~CommandBuffer()
	{
	}

	void CommandBuffer::ResetState()
	{
		_dirty = ~0u;
        _dirtySets = ~0u;
		_dirtyVbos = ~0u;
		memset(_vbo.buffers, 0, sizeof(_vbo.buffers));
        memset(&_bindings, 0, sizeof(_bindings));
	}

	void CommandBuffer::SetVertexBuffer(GpuBuffer* buffer, uint32_t binding,uint64_t offset, VertexInputRate inputRate)
	{
		ALIMER_ASSERT(binding < MaxVertexBufferBindings);
		ALIMER_ASSERT(buffer);
		ALIMER_ASSERT(any(buffer->GetBufferUsage() & BufferUsage::Vertex));

		if (_vbo.buffers[binding] != buffer
			|| _vbo.offsets[binding] != offset)
		{
			_dirtyVbos |= 1u << binding;
		}

		uint64_t stride = buffer->GetElementSize();
		if (_vbo.strides[binding] != stride
			|| _vbo.inputRates[binding] != inputRate)
		{
			SetDirty(COMMAND_BUFFER_DIRTY_STATIC_VERTEX_BIT);
		}

		_vbo.buffers[binding] = buffer;
		_vbo.offsets[binding] = offset;
		_vbo.strides[binding] = stride;
		_vbo.inputRates[binding] = inputRate;
	}

    void CommandBuffer::SetUniformBuffer(uint32_t set, uint32_t binding, const GpuBuffer* buffer)
    {
        ALIMER_ASSERT(set < MaxDescriptorSets);
        ALIMER_ASSERT(binding < MaxBindingsPerSet);
        ALIMER_ASSERT(any(buffer->GetBufferUsage() & BufferUsage::Uniform));
        auto &b = _bindings.bindings[set][binding];

        uint64_t range = buffer->GetSize();
        if (buffer == b.buffer.buffer
            && b.buffer.offset == 0
            && b.buffer.range == range)
            return;

        b.buffer = { buffer, 0, range };
        _dirtySets |= 1u << set;
    }

	void CommandBuffer::Draw(PrimitiveTopology topology, uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t baseInstance)
	{
		DrawCore(topology, vertexCount, instanceCount, vertexStart, baseInstance);
	}

	void CommandBuffer::DrawIndexed(PrimitiveTopology topology, uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex)
	{
		DrawIndexedCore(topology, indexCount, instanceCount, startIndex);
	}
}
