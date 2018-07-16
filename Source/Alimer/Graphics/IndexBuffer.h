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

#pragma once

#include "../Graphics/GpuBuffer.h"

namespace Alimer
{
    /// Defines a GPU IndexBuffer class.
    class IndexBuffer final : public GpuBuffer
    {
    public:
        /// Constructor.
        IndexBuffer(Graphics* graphics, uint32_t indexCount, IndexType indexType);

        /// Destructor.
        virtual ~IndexBuffer() override;

        /// Return number of indices.
        uint32_t GetIndexCount() const { return _indexCount; }

        /// Return single element type.
        IndexType GetIndexType() const { return _indexType; }
        /// Return size of index in bytes.
        uint32_t GetIndexSize() const { return _stride; }
    private:
        uint32_t _indexCount;
        IndexType _indexType;
    };
}
