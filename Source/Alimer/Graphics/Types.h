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

#include "../Core/Platform.h"
#include "../Math/Math.h"
#include "../Math/Color.h"
#include <string>

namespace Alimer
{
    static constexpr uint32_t RemainingMipLevels  = ~0U;
    static constexpr uint32_t RemainingArrayLayers = ~0U;
    static constexpr uint32_t MaxViewportsAndScissors = 16u;
    static constexpr uint32_t MaxDescriptorSets = 4u;
    static constexpr uint32_t MaxBindingsPerSet = 14u;
    static constexpr uint32_t MaxVertexAttributes = 16u;
    static constexpr uint32_t MaxVertexBufferBindings = 4u;
    static constexpr uint32_t MaxColorAttachments = 8u;

    /// Enum describing the Graphics backend.
    enum class GraphicsBackend : uint32_t
    {
        /// Best device supported for running platform.
        Default,
        /// Empty/Headless device type.
        Empty,
        /// Vulkan backend.
        Vulkan,
        /// DirectX 11.1+ backend.
        Direct3D11,
        /// DirectX 12 backend.
        Direct3D12,
        /// Metal backend.
        Metal,
        /// OpenGL backend.
        OpenGL,
    };

    /// Enum describing the number of samples.
    enum class SampleCount : uint32_t
    {
        /// 1 sample (no multi-sampling).
        Count1 = 1,
        /// 2 Samples.
        Count2 = 2,
        /// 4 Samples.
        Count4 = 4,
        /// 8 Samples.
        Count8 = 8,
        /// 16 Samples.
        Count16 = 16,
        /// 32 Samples.
        Count32 = 32,
    };

    enum class ResourceUsage : unsigned
    {
        Default,
        Immutable,
        Dynamic,
        Staging
    };

    enum class LoadAction
    {
        DontCare,
        Load,
        Clear
    };

    enum class StoreAction
    {
        DontCare,
        Store
    };

    /// Primitive topology.
    enum class PrimitiveTopology : uint32_t
    {
        Points,
        Lines,
        LineStrip,
        Triangles,
        TriangleStrip,
        Count
    };

    enum class BufferUsage : uint32_t
    {
        None = 0,
        Vertex = 1 << 0,
        Index = 1 << 1,
        Uniform = 1 << 2,
        Storage = 1 << 3,
        Indirect = 1 << 4,
    };
    ALIMER_BITMASK(BufferUsage);

    enum class VertexFormat : uint32_t
    {
        Invalid = 0,
        Float,
        Float2,
        Float3,
        Float4,
        Byte4,
        Byte4N,
        UByte4,
        UByte4N,
        Short2,
        Short2N,
        Short4,
        Short4N,
        Count
    };
    
    /// VertexInputRate
    enum class VertexInputRate
    {
        Vertex,
        Instance
    };

    enum class ParamDataType
    {
        Unknown = 0,
        Void,
        Boolean,
        Char,
        Int,
        UInt,
        Int64,
        UInt64,
        Half,
        Float,
        Double,
        Struct
    };

    enum class ParamAccess
    {
        Read,
        Write,
        ReadWrite
    };

    enum class ResourceParamType
    {
        Input = 0,
        Output = 1,
    };

    /// Defines shader stage
    enum class ShaderStage : uint32_t
    {
        Vertex = 0,
        TessControl = 1,
        TessEvaluation = 2,
        Geometry = 3,
        Fragment = 4,
        Compute = 5,
        Count
    };

    /// Defines shader stage usage.
    enum class ShaderStageUsage : uint32_t
    {
        None = 0,
        Vertex = 1 << 0,
        TessControl = 1 << 1,
        TessEvaluation = 1 << 2,
        Geometry = 1 << 3,
        Fragment = 1 << 4,
        Compute = 1 << 5,
        AllGraphics = (Vertex | TessControl | TessEvaluation | Geometry | Fragment),
        All = (AllGraphics | Compute),
    };
    ALIMER_BITMASK(ShaderStageUsage);

    struct BufferDescriptor
    {
        /// Buffer resource usage.
        ResourceUsage resourceUsage = ResourceUsage::Default;

        /// Buffer usage.
        BufferUsage usage = BufferUsage::None;

        /// Size in bytes of buffer.
        uint64_t size = 0;

        /// Size of each individual element in the buffer, in bytes. 
        uint32_t stride = 0;
    };

    struct PipelineResource
    {
        std::string name;
        ShaderStageUsage stages;
        ResourceParamType resourceType;
        ParamDataType dataType;
        ParamAccess access;
        uint32_t set;
        uint32_t binding;
        uint32_t location;
        uint32_t vecSize;
        uint32_t arraySize;
        uint32_t offset;
        uint32_t size;
    };

    struct ShaderReflection
    {
        ShaderStage stage;
        uint32_t inputMask = 0;
        uint32_t outputMask = 0;

        std::vector<PipelineResource> resources;
    };

    enum class VertexElementSemantic : uint32_t
    {
        Position = 0,
        Normal,
        Binormal,
        Tangent,
        BlendWeight,
        BlendIndices,
        Color0,
        Color1,
        Color2,
        Color3,
        Texcoord0,
        Texcoord1,
        Texcoord2,
        Texcoord3,
        Texcoord4,
        Texcoord5,
        Texcoord6,
        Texcoord7,
        Count
    };


    struct VertexAttributeDescriptor
    {
        VertexElementSemantic   semantic = VertexElementSemantic::Position;
        VertexFormat            format = VertexFormat::Invalid;
        uint32_t                offset = 0;
    };

    ALIMER_API uint32_t GetVertexFormatSize(VertexFormat format);
    ALIMER_API const char* EnumToString(ResourceUsage usage);
    ALIMER_API const char* EnumToString(BufferUsage usage);
    ALIMER_API const char* EnumToString(ShaderStage stage);
    ALIMER_API const char* EnumToString(VertexElementSemantic semantic);
}
