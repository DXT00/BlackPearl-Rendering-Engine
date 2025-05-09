#pragma once
#include <functional>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include "BlackPearl/RHI/Common/Containers.h"
#include "BlackPearl/Core/Memory.h"
#include "BlackPearl/Math/vector.h"
namespace BlackPearl {
    using namespace math;
    class ITexture;
    enum class FilterMode : uint8_t {
        Linear,
        Nearest,
        Nearest_Mip_Nearnest,
        Linear_Mip_Linear
    };
    enum TextureType {
        None,
        DiffuseMap,
        SpecularMap,
        EmissionMap,
        NormalMap,
        HeightMap,
        CubeMap,
        DepthMap,
        AoMap,
        RoughnessMap,
        MentallicMap,
        OpacityMap,
        Image2DMap
    };

    enum class TextureAccess {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };
  
    // describes a 2D section of a single mip level + single slice of a texture
    enum EClearType
    {
        CT_None = 0x0,
        CT_Depth = 0x1,
        CT_Stencil = 0x2,
        CT_Color = 0x4,
        CT_DepthStencil = CT_Depth | CT_Stencil,
    };
    /** Flags used for texture creation */
    enum class ETextureCreateFlags : uint64_t
    {
        None = 0,

        // Texture can be used as a render target
        RenderTargetable = 1ull << 0,
        // Texture can be used as a resolve target
        ResolveTargetable = 1ull << 1,
        // Texture can be used as a depth-stencil target.
        DepthStencilTargetable = 1ull << 2,
        // Texture can be used as a shader resource.
        ShaderResource = 1ull << 3,
        // Texture is encoded in sRGB gamma space
        SRGB = 1ull << 4,
        // Texture data is writable by the CPU
        CPUWritable = 1ull << 5,
        // Texture will be created with an un-tiled format
        NoTiling = 1ull << 6,
        // Texture will be used for video decode
        VideoDecode = 1ull << 7,
        // Texture that may be updated every frame
        Dynamic = 1ull << 8,
        // Texture will be used as a render pass attachment that will be read from
        InputAttachmentRead = 1ull << 9,
        /** Texture represents a foveation attachment */
        Foveation = 1ull << 10,
        // Prefer 3D internal surface tiling mode for volume textures when possible
        Tiling3D = 1ull << 11,
        // This texture has no GPU or CPU backing. It only exists in tile memory on TBDR GPUs (i.e., mobile).
        Memoryless = 1ull << 12,
        // Create the texture with the flag that allows mip generation later, only applicable to D3D11
        GenerateMipCapable = 1ull << 13,
        // The texture can be partially allocated in fastvram
        FastVRAMPartialAlloc = 1ull << 14,
        // Do not create associated shader resource view, only applicable to D3D11 and D3D12
        DisableSRVCreation = 1ull << 15,
        // Do not allow Delta Color Compression (DCC) to be used with this texture
        DisableDCC = 1ull << 16,
        // UnorderedAccessView (DX11 only)
        // Warning: Causes additional synchronization between draw calls when using a render target allocated with this flag, use sparingly
        // See: GCNPerformanceTweets.pdf Tip 37
        UAV = 1ull << 17,
        // Render target texture that will be displayed on screen (back buffer)
        Presentable = 1ull << 18,
        // Texture data is accessible by the CPU
        CPUReadback = 1ull << 19,
        // Texture was processed offline (via a texture conversion process for the current platform)
        OfflineProcessed = 1ull << 20,
        // Texture needs to go in fast VRAM if available (HINT only)
        FastVRAM = 1ull << 21,
        // by default the texture is not showing up in the list - this is to reduce clutter, using the FULL option this can be ignored
        HideInVisualizeTexture = 1ull << 22,
        // Texture should be created in virtual memory, with no physical memory allocation made
        // You must make further calls to RHIVirtualTextureSetFirstMipInMemory to allocate physical memory
        // and RHIVirtualTextureSetFirstMipVisible to map the first mip visible to the GPU
        Virtual = 1ull << 23,
        // Creates a RenderTargetView for each array slice of the texture
        // Warning: if this was specified when the resource was created, you can't use SV_RenderTargetArrayIndex to route to other slices!
        TargetArraySlicesIndependently = 1ull << 24,
        // Texture that may be shared with DX9 or other devices
        Shared = 1ull << 25,
        // RenderTarget will not use full-texture fast clear functionality.
        NoFastClear = 1ull << 26,
        // Texture is a depth stencil resolve target
        DepthStencilResolveTarget = 1ull << 27,
        // Flag used to indicted this texture is a streamable 2D texture, and should be counted towards the texture streaming pool budget.
        Streamable = 1ull << 28,
        // Render target will not FinalizeFastClear; Caches and meta data will be flushed, but clearing will be skipped (avoids potentially trashing metadata)
        NoFastClearFinalize = 1ull << 29,
        /** Texture needs to support atomic operations */
        Atomic64Compatible = 1ull << 30,
        // Workaround for 128^3 volume textures getting bloated 4x due to tiling mode on some platforms.
        ReduceMemoryWithTilingMode = 1ull << 31,
        /** Texture needs to support atomic operations */
        AtomicCompatible = 1ull << 33,
        /** Texture should be allocated for external access. Vulkan only */
        External = 1ull << 34,
        /** Don't automatically transfer across GPUs in multi-GPU scenarios.  For example, if you are transferring it yourself manually. */
        MultiGPUGraphIgnore = 1ull << 35,
        /** EXPERIMENTAL: Allow the texture to be created as a reserved (AKA tiled/sparse/virtual) resource internally, without physical memory backing. */
        ReservedResource = 1ull << 37,
        /** EXPERIMENTAL: Used with ReservedResource flag to immediately allocate and commit memory on creation. May use N small physical memory allocations instead of a single large one. */
        ImmediateCommit = 1ull << 38,

        /** Don't lump this texture with streaming memory when tracking total texture allocation sizes */
        ForceIntoNonStreamingMemoryTracking = 1ull << 39,
    };
    /*    uniform buffer   */
        /** The base type of a value in a shader parameter structure. */
    enum EUniformBufferBaseType : uint8_t
    {
        UBMT_INVALID,

        // Invalid type when trying to use bool, to have explicit error message to programmer on why
        // they shouldn't use bool in shader parameter structures.
        UBMT_BOOL,

        // Parameter types.
        UBMT_INT32,
        UBMT_UINT32,
        UBMT_FLOAT32,

        // RHI resources not tracked by render graph.
        UBMT_TEXTURE,
        UBMT_SRV,
        UBMT_UAV,
        UBMT_SAMPLER,

        // Resources tracked by render graph.
        UBMT_RDG_TEXTURE,
        UBMT_RDG_TEXTURE_ACCESS,
        UBMT_RDG_TEXTURE_ACCESS_ARRAY,
        UBMT_RDG_TEXTURE_SRV,
        UBMT_RDG_TEXTURE_UAV,
        UBMT_RDG_BUFFER_ACCESS,
        UBMT_RDG_BUFFER_ACCESS_ARRAY,
        UBMT_RDG_BUFFER_SRV,
        UBMT_RDG_BUFFER_UAV,
        UBMT_RDG_UNIFORM_BUFFER,

        // Nested structure.
        UBMT_NESTED_STRUCT,

        // Structure that is nested on C++ side, but included on shader side.
        UBMT_INCLUDED_STRUCT,

        // GPU Indirection reference of struct, like is currently named Uniform buffer.
        UBMT_REFERENCED_STRUCT,

        // Structure dedicated to setup render targets for a rasterizer pass.
        UBMT_RENDER_TARGET_BINDING_SLOTS,

        EUniformBufferBaseType_Num,
        EUniformBufferBaseType_NumBits = 5,
    };
    
    /** The layout of a uniform buffer in memory. */
    struct UniformBufferLayoutItem {
        UniformBufferLayoutItem(EUniformBufferBaseType type, void* varying)
            :type(type),
            varying(varying)
        {
        }

        EUniformBufferBaseType type;
        void* varying;
    };


    struct UniformBufferLayout
    {
        UniformBufferLayout(std::initializer_list<UniformBufferLayoutItem> items) {}
        //    for (const UniformBufferLayoutItem& item : items)
        //    {
        //        switch (item.type)
        //        {
        //        case EUniformBufferBaseType::UBMT_INT32:
        //            UBInt.push_back(*(static_cast<int32_t*>(item.varying)));
        //            break;
        //        case EUniformBufferBaseType::UBMT_FLOAT32:
        //            UBFloat.push_back(*(static_cast<float*>(item.varying)));
        //            break;
        //            /*case EUniformBufferBaseType::UBMT_FLOAT32:
        //                UBFloat.push_back(*(static_cast<float*>(item.varying)));
        //                break;
        //            case EUniformBufferBaseType::UBMT_FLOAT32:
        //                UBFloat.push_back(*(static_cast<float*>(item.varying)));
        //                break;*/
        //        default:
        //            break;
        //        }
        //    }
        //}
        //std::vector<int32_t>   UBInt;
        //std::vector<float> UBFloat;
        //std::vector<float2> UBFloat2;
        //std::vector<float3> UBFloat3;
        //std::vector<float4> UBFloat4;
        //std::vector<float4x4> UBFloat4x4;
        //std::vector<float3x3> UBFloat3x3;

    };
    enum
    {
	    OGL_MAX_UNIFORM_BUFFER_BINDINGS = 12,	
	    OGL_FIRST_UNIFORM_BUFFER = 0,			
	    OGL_UAV_NOT_SUPPORTED_FOR_GRAPHICS_UNIT = -1, 
    };

 /** Maximum number of miplevels in a texture. */
    enum { MAX_TEXTURE_MIP_COUNT = 15 };

    /** Maximum number of static/skeletal mesh LODs */
    enum { MAX_MESH_LOD_COUNT = 8 };


    /** The maximum number of vertex elements which can be used by a vertex declaration. */
    enum
    {
        MaxVertexElementCount = 17,
        MaxVertexElementCount_NumBits = 5,
    };
    enum class EGpuVendorId : uint32_t
    {
        Unknown = 0xffffffff,
        NotQueried = 0,

        Amd = 0x1002,
        ImgTec = 0x1010,
        Nvidia = 0x10DE,
        Arm = 0x13B5,
        Broadcom = 0x14E4,
        Qualcomm = 0x5143,
        Intel = 0x8086,
        Apple = 0x106B,
        Vivante = 0x7a05,
        VeriSilicon = 0x1EB1,
        SamsungAMD = 0x144D,
        Microsoft = 0x1414,

        Kazan = 0x10003,	// VkVendorId
        Codeplay = 0x10004,	// VkVendorId
        Mesa = 0x10005,	// VkVendorId
    };

   namespace ObjectTypes
    {
        constexpr uint32_t SharedHandle = 0x00000001;

        constexpr uint32_t D3D11_Device = 0x00010001;
        constexpr uint32_t D3D11_DeviceContext = 0x00010002;
        constexpr uint32_t D3D11_Resource = 0x00010003;
        constexpr uint32_t D3D11_Buffer = 0x00010004;
        constexpr uint32_t D3D11_RenderTargetView = 0x00010005;
        constexpr uint32_t D3D11_DepthStencilView = 0x00010006;
        constexpr uint32_t D3D11_ShaderResourceView = 0x00010007;
        constexpr uint32_t D3D11_UnorderedAccessView = 0x00010008;

        constexpr uint32_t D3D12_Device = 0x00020001;
        constexpr uint32_t D3D12_CommandQueue = 0x00020002;
        constexpr uint32_t D3D12_GraphicsCommandList = 0x00020003;
        constexpr uint32_t D3D12_Resource = 0x00020004;
        constexpr uint32_t D3D12_RenderTargetViewDescriptor = 0x00020005;
        constexpr uint32_t D3D12_DepthStencilViewDescriptor = 0x00020006;
        constexpr uint32_t D3D12_ShaderResourceViewGpuDescripror = 0x00020007;
        constexpr uint32_t D3D12_UnorderedAccessViewGpuDescripror = 0x00020008;
        constexpr uint32_t D3D12_RootSignature = 0x00020009;
        constexpr uint32_t D3D12_PipelineState = 0x0002000a;
        constexpr uint32_t D3D12_CommandAllocator = 0x0002000b;

        constexpr uint32_t VK_Device = 0x00030001;
        constexpr uint32_t VK_PhysicalDevice = 0x00030002;
        constexpr uint32_t VK_Instance = 0x00030003;
        constexpr uint32_t VK_Queue = 0x00030004;
        constexpr uint32_t VK_CommandBuffer = 0x00030005;
        constexpr uint32_t VK_DeviceMemory = 0x00030006;
        constexpr uint32_t VK_Buffer = 0x00030007;
        constexpr uint32_t VK_Image = 0x00030008;
        constexpr uint32_t VK_ImageView = 0x00030009;
        constexpr uint32_t VK_AccelerationStructureKHR = 0x0003000a;
        constexpr uint32_t VK_Sampler = 0x0003000b;
        constexpr uint32_t VK_ShaderModule = 0x0003000c;
        constexpr uint32_t VK_RenderPass = 0x0003000d;
        constexpr uint32_t VK_Framebuffer = 0x0003000e;
        constexpr uint32_t VK_DescriptorPool = 0x0003000f;
        constexpr uint32_t VK_DescriptorSetLayout = 0x00030010;
        constexpr uint32_t VK_DescriptorSet = 0x00030011;
        constexpr uint32_t VK_PipelineLayout = 0x00030012;
        constexpr uint32_t VK_Pipeline = 0x00030013;
        constexpr uint32_t VK_Micromap = 0x00030014;
    };

  

    struct RHIObject
    {
        union {
            uint64_t integer;
            void* pointer;
        };

        RHIObject(uint64_t i) : integer(i) { }  // NOLINT(cppcoreguidelines-pro-type-member-init)
        RHIObject(void* p) : pointer(p) { }     // NOLINT(cppcoreguidelines-pro-type-member-init)

        template<typename T> operator T* () const { return static_cast<T*>(pointer); }
    };

    struct MemoryRequirements
    {
        uint64_t size = 0;
        uint64_t alignment = 0;
    };



    enum class GraphicsAPI : uint8_t
    {
        D3D11,
        D3D12,
        VULKAN
    };

    constexpr uint64_t c_VersionSubmittedFlag = 0x8000000000000000;
    constexpr uint32_t c_VersionQueueShift = 60;
    constexpr uint32_t c_VersionQueueMask = 0x7;
    constexpr uint64_t c_VersionIDMask = 0x0FFFFFFFFFFFFFFF;


    enum class Feature : uint8_t
    {
        DeferredCommandLists,
        SinglePassStereo,
        RayTracingAccelStruct,
        RayTracingPipeline,
        RayTracingOpacityMicromap,
        RayQuery,
        ShaderExecutionReordering,
        FastGeometryShader,
        Meshlets,
        ConservativeRasterization,
        VariableRateShading,
        ShaderSpecializations,
        VirtualResources,
        ComputeQueue,
        CopyQueue,
        ConstantBufferRanges
    };

    struct VariableRateShadingFeatureInfo
    {
        uint32_t shadingRateImageTileSize;
    };

    enum class CommandQueue : uint8_t
    {
        Graphics = 0,
        Compute,
        Copy,

        Count
    };

    constexpr uint64_t MakeVersion(uint64_t id, CommandQueue queue, bool submitted)
    {
        uint64_t result = (id & c_VersionIDMask) | (uint64_t(queue) << c_VersionQueueShift);
        if (submitted) result |= c_VersionSubmittedFlag;
        return result;
    }

    constexpr uint64_t VersionGetInstance(uint64_t version)
    {
        return version & c_VersionIDMask;
    }

    constexpr CommandQueue VersionGetQueue(uint64_t version)
    {
        return CommandQueue((version >> c_VersionQueueShift) & c_VersionQueueMask);
    }

    constexpr bool VersionGetSubmitted(uint64_t version)
    {
        return (version & c_VersionSubmittedFlag) != 0;
    }

    static constexpr uint32_t c_MaxRenderTargets = 8;
    static constexpr uint32_t c_MaxViewports = 16;
    static constexpr uint32_t c_MaxVertexAttributes = 16;
    static constexpr uint32_t c_MaxBindingLayouts = 20;
    static constexpr uint32_t c_MaxBindingsPerLayout = 128;
    static constexpr uint32_t c_MaxVolatileConstantBuffersPerLayout = 6;
    static constexpr uint32_t c_MaxVolatileConstantBuffers = 32;
    static constexpr uint32_t c_MaxPushConstantSize = 128; // D3D12: root signature is 256 bytes max., Vulkan: 128 bytes of push constants guaranteed
    static constexpr uint32_t c_ConstantBufferOffsetSizeAlignment = 256; // Partially bound constant buffers must have offsets aligned to this and sizes multiple of this

    template <typename T>
    using BindingVector = static_vector<T, c_MaxBindingLayouts>;

#define NVRHI_ENUM_CLASS_FLAG_OPERATORS(T) \
    inline T operator | (T a, T b) { return T(uint32_t(a) | uint32_t(b)); } \
    inline T operator & (T a, T b) { return T(uint32_t(a) & uint32_t(b)); } /* NOLINT(bugprone-macro-parentheses) */ \
    inline T operator ~ (T a) { return T(~uint32_t(a)); } /* NOLINT(bugprone-macro-parentheses) */ \
    inline bool operator !(T a) { return uint32_t(a) == 0; } \
    inline bool operator ==(T a, uint32_t b) { return uint32_t(a) == b; } \
    inline bool operator !=(T a, uint32_t b) { return uint32_t(a) != b; }


    struct Color
    {
        float r, g, b, a;

        Color() : r(0.f), g(0.f), b(0.f), a(0.f) { }
        Color(float c) : r(c), g(c), b(c), a(c) { }
        Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) { }

        bool operator ==(const Color& _b) const { return r == _b.r && g == _b.g && b == _b.b && a == _b.a; }
        bool operator !=(const Color& _b) const { return !(*this == _b); }
    };

   
    enum class BlendFactor : uint8_t
    {
        Zero = 1,
        One = 2,
        SrcColor = 3,
        InvSrcColor = 4,
        SrcAlpha = 5,
        InvSrcAlpha = 6,
        DstAlpha = 7,
        InvDstAlpha = 8,
        DstColor = 9,
        InvDstColor = 10,
        SrcAlphaSaturate = 11,
        ConstantColor = 14,
        InvConstantColor = 15,
        Src1Color = 16,
        InvSrc1Color = 17,
        Src1Alpha = 18,
        InvSrc1Alpha = 19,

        // Vulkan names
        OneMinusSrcColor = InvSrcColor,
        OneMinusSrcAlpha = InvSrcAlpha,
        OneMinusDstAlpha = InvDstAlpha,
        OneMinusDstColor = InvDstColor,
        OneMinusConstantColor = InvConstantColor,
        OneMinusSrc1Color = InvSrc1Color,
        OneMinusSrc1Alpha = InvSrc1Alpha,
    };

    enum class BlendOp : uint8_t
    {
        Add = 1,
        Subrtact = 2,
        ReverseSubtract = 3,
        Min = 4,
        Max = 5
    };
    enum class ColorMask : uint8_t
    {
        // These values are equal to their counterparts in DX11, DX12, and Vulkan.
        Red = 1,
        Green = 2,
        Blue = 4,
        Alpha = 8,
        All = 0xF


    };

  
    struct BlendState
    {
        struct RenderTarget
        {
            bool        blendEnable = false;
            BlendFactor srcBlend = BlendFactor::One;
            BlendFactor destBlend = BlendFactor::Zero;
            BlendOp     blendOp = BlendOp::Add;
            BlendFactor srcBlendAlpha = BlendFactor::One;
            BlendFactor destBlendAlpha = BlendFactor::Zero;
            BlendOp     blendOpAlpha = BlendOp::Add;

            ColorMask   colorWriteMask = ColorMask::All;
            /*glBlendFuncSeparatei �ر������� ����ȾĿ�꣨MRT����Ⱦ�У���Ϊÿ����ȾĿ�������Ҫ��ͬ�Ļ����Ϊ*/
            bool bSeparateAlphaBlendEnable = false;
            bool colorWriteMaskRed() const{ return (uint8_t)colorWriteMask & (uint8_t)ColorMask::Red; }
            bool colorWriteMaskGreen() const { return (uint8_t)colorWriteMask & (uint8_t)ColorMask::Green; }
            bool colorWriteMaskBlue() const { return (uint8_t)colorWriteMask & (uint8_t)ColorMask::Blue; }

            bool colorWriteMaskAlpha() const { return (uint8_t)colorWriteMask & (uint8_t)ColorMask::Alpha; }

            constexpr RenderTarget& setBlendEnable(bool enable) { blendEnable = enable; return *this; }
            constexpr RenderTarget& enableBlend() { blendEnable = true; return *this; }
            constexpr RenderTarget& disableBlend() { blendEnable = false; return *this; }
            constexpr RenderTarget& setSrcBlend(BlendFactor value) { srcBlend = value; return *this; }
            constexpr RenderTarget& setDestBlend(BlendFactor value) { destBlend = value; return *this; }
            constexpr RenderTarget& setBlendOp(BlendOp value) { blendOp = value; return *this; }
            constexpr RenderTarget& setSrcBlendAlpha(BlendFactor value) { srcBlendAlpha = value; return *this; }
            constexpr RenderTarget& setDestBlendAlpha(BlendFactor value) { destBlendAlpha = value; return *this; }
            constexpr RenderTarget& setBlendOpAlpha(BlendOp value) { blendOpAlpha = value; return *this; }
            constexpr RenderTarget& setColorWriteMask(ColorMask value) { colorWriteMask = value; return *this; }

            [[nodiscard]] bool usesConstantColor() const;

            constexpr bool operator ==(const RenderTarget& other) const
            {
                return blendEnable == other.blendEnable
                    && srcBlend == other.srcBlend
                    && destBlend == other.destBlend
                    && blendOp == other.blendOp
                    && srcBlendAlpha == other.srcBlendAlpha
                    && destBlendAlpha == other.destBlendAlpha
                    && blendOpAlpha == other.blendOpAlpha
                    && colorWriteMask == other.colorWriteMask;
            }

            constexpr bool operator !=(const RenderTarget& other) const
            {
                return !(*this == other);
            }
        };

        RenderTarget targets[c_MaxRenderTargets];
        //alphaToCoverageEnable ����MSAA
        /*
        * �ڶ��ز�������ݣ�MSAA, Multi-Sample Anti-Aliasing���У�ÿ�����ؿ����ж��������ÿ�����������Լ��� alpha ֵ��GL_SAMPLE_ALPHA_TO_COVERAGE �������Ǿ����Ƿ����ƬԪ�� alpha ֵ��Ӱ�츲���ʣ����Ƿ���� alpha ֵ���������صĸ���״̬��
        */
        bool alphaToCoverageEnable = false;

        constexpr BlendState& setRenderTarget(uint32_t index, const RenderTarget& target) { targets[index] = target; return *this; }
        constexpr BlendState& setAlphaToCoverageEnable(bool enable) { alphaToCoverageEnable = enable; return *this; }
        constexpr BlendState& enableAlphaToCoverage() { alphaToCoverageEnable = true; return *this; }
        constexpr BlendState& disableAlphaToCoverage() { alphaToCoverageEnable = false; return *this; }

        [[nodiscard]] bool usesConstantColor(uint32_t numTargets) const;

        constexpr bool operator ==(const BlendState& other) const
        {
            if (alphaToCoverageEnable != other.alphaToCoverageEnable)
                return false;

            for (uint32_t i = 0; i < c_MaxRenderTargets; ++i)
            {
                if (targets[i] != other.targets[i])
                    return false;
            }

            return true;
        }

        constexpr bool operator !=(const BlendState& other) const
        {
            return !(*this == other);
        }
    };
    enum class StencilOp : uint8_t
    {
        Keep = 1,
        Zero = 2,
        Replace = 3,
        IncrementAndClamp = 4,
        DecrementAndClamp = 5,
        Invert = 6,
        IncrementAndWrap = 7,
        DecrementAndWrap = 8
    };

    enum class ComparisonFunc : uint8_t
    {
        Never = 1,
        Less = 2,
        Equal = 3,
        LessOrEqual = 4,
        Greater = 5,
        NotEqual = 6,
        GreaterOrEqual = 7,
        Always = 8
    };

    struct DepthStencilState
    {
        struct StencilOpDesc
        {
            StencilOp failOp = StencilOp::Keep;
            StencilOp depthFailOp = StencilOp::Keep;
            StencilOp passOp = StencilOp::Keep;
            ComparisonFunc stencilFunc = ComparisonFunc::Always;

            constexpr StencilOpDesc& setFailOp(StencilOp value) { failOp = value; return *this; }
            constexpr StencilOpDesc& setDepthFailOp(StencilOp value) { depthFailOp = value; return *this; }
            constexpr StencilOpDesc& setPassOp(StencilOp value) { passOp = value; return *this; }
            constexpr StencilOpDesc& setStencilFunc(ComparisonFunc value) { stencilFunc = value; return *this; }
        };
        bool            bTwoSidedStencilMode = false;
        bool            depthTestEnable = true;
        bool            depthWriteEnable = true;
        ComparisonFunc  depthFunc = ComparisonFunc::Less;
        bool            stencilEnable = false;
        uint8_t         stencilReadMask = 0xff;
        uint8_t         stencilWriteMask = 0xff;
        uint8_t         stencilRefValue = 0;
        StencilOpDesc   frontFaceStencil;
        StencilOpDesc   backFaceStencil;

        constexpr DepthStencilState& setDepthTestEnable(bool value) { depthTestEnable = value; return *this; }
        constexpr DepthStencilState& enableDepthTest() { depthTestEnable = true; return *this; }
        constexpr DepthStencilState& disableDepthTest() { depthTestEnable = false; return *this; }
        constexpr DepthStencilState& setDepthWriteEnable(bool value) { depthWriteEnable = value; return *this; }
        constexpr DepthStencilState& enableDepthWrite() { depthWriteEnable = true; return *this; }
        constexpr DepthStencilState& disableDepthWrite() { depthWriteEnable = false; return *this; }
        constexpr DepthStencilState& setDepthFunc(ComparisonFunc value) { depthFunc = value; return *this; }
        constexpr DepthStencilState& setStencilEnable(bool value) { stencilEnable = value; return *this; }
        constexpr DepthStencilState& enableStencil() { stencilEnable = true; return *this; }
        constexpr DepthStencilState& disableStencil() { stencilEnable = false; return *this; }
        constexpr DepthStencilState& setStencilReadMask(uint8_t value) { stencilReadMask = value; return *this; }
        constexpr DepthStencilState& setStencilWriteMask(uint8_t value) { stencilWriteMask = value; return *this; }
        constexpr DepthStencilState& setStencilRefValue(uint8_t value) { stencilRefValue = value; return *this; }
        constexpr DepthStencilState& setFrontFaceStencil(const StencilOpDesc& value) { frontFaceStencil = value; return *this; }
        constexpr DepthStencilState& setBackFaceStencil(const StencilOpDesc& value) { backFaceStencil = value; return *this; }
    };


    enum class FormatSupport : uint32_t
    {
        None = 0,

        Buffer = 0x00000001,
        IndexBuffer = 0x00000002,
        VertexBuffer = 0x00000004,

        Texture = 0x00000008,
        DepthStencil = 0x00000010,
        RenderTarget = 0x00000020,
        Blendable = 0x00000040,

        ShaderLoad = 0x00000080,
        ShaderSample = 0x00000100,
        ShaderUavLoad = 0x00000200,
        ShaderUavStore = 0x00000400,
        ShaderAtomic = 0x00000800,
    };

    NVRHI_ENUM_CLASS_FLAG_OPERATORS(FormatSupport)


    enum class HeapType : uint8_t
    {
        DeviceLocal,
        Upload,
        Readback
    };

    struct HeapDesc
    {
        uint64_t capacity = 0;
        HeapType type;
        std::string debugName;

        constexpr HeapDesc& setCapacity(uint64_t value) { capacity = value; return *this; }
        constexpr HeapDesc& setType(HeapType value) { type = value; return *this; }
        HeapDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
    };


    enum class RasterFillMode : uint8_t
    {
        Solid,
        Wireframe,

        // Vulkan names
        Fill = Solid,
        Line = Wireframe
    };
    enum class RasterCullMode : uint8_t
    {
        Back,
        Front,
        None
    };

    struct RasterState
    {
        RasterFillMode fillMode = RasterFillMode::Solid;
        RasterCullMode cullMode = RasterCullMode::Back;
        bool frontCounterClockwise = false;
        bool depthClipEnable = false;
        bool scissorEnable = false;
        bool multisampleEnable = false;
        bool antialiasedLineEnable = false;
        int depthBias = 0;
        float depthBiasClamp = 0.f;
        float slopeScaledDepthBias = 0.f;

        // Extended rasterizer state supported by Maxwell
        // In D3D11, use NvAPI_D3D11_CreateRasterizerState to create such rasterizer state.
        uint8_t forcedSampleCount = 0;
        bool programmableSamplePositionsEnable = false;
        bool conservativeRasterEnable = false;
        bool quadFillEnable = false;
        char samplePositionsX[16]{};
        char samplePositionsY[16]{};

        constexpr RasterState& setFillMode(RasterFillMode value) { fillMode = value; return *this; }
        constexpr RasterState& setFillSolid() { fillMode = RasterFillMode::Solid; return *this; }
        constexpr RasterState& setFillWireframe() { fillMode = RasterFillMode::Wireframe; return *this; }
        constexpr RasterState& setCullMode(RasterCullMode value) { cullMode = value; return *this; }
        constexpr RasterState& setCullBack() { cullMode = RasterCullMode::Back; return *this; }
        constexpr RasterState& setCullFront() { cullMode = RasterCullMode::Front; return *this; }
        constexpr RasterState& setCullNone() { cullMode = RasterCullMode::None; return *this; }
        constexpr RasterState& setFrontCounterClockwise(bool value) { frontCounterClockwise = value; return *this; }
        constexpr RasterState& setDepthClipEnable(bool value) { depthClipEnable = value; return *this; }
        constexpr RasterState& enableDepthClip() { depthClipEnable = true; return *this; }
        constexpr RasterState& disableDepthClip() { depthClipEnable = false; return *this; }
        constexpr RasterState& setScissorEnable(bool value) { scissorEnable = value; return *this; }
        constexpr RasterState& enableScissor() { scissorEnable = true; return *this; }
        constexpr RasterState& disableScissor() { scissorEnable = false; return *this; }
        constexpr RasterState& setMultisampleEnable(bool value) { multisampleEnable = value; return *this; }
        constexpr RasterState& enableMultisample() { multisampleEnable = true; return *this; }
        constexpr RasterState& disableMultisample() { multisampleEnable = false; return *this; }
        constexpr RasterState& setAntialiasedLineEnable(bool value) { antialiasedLineEnable = value; return *this; }
        constexpr RasterState& enableAntialiasedLine() { antialiasedLineEnable = true; return *this; }
        constexpr RasterState& disableAntialiasedLine() { antialiasedLineEnable = false; return *this; }
        constexpr RasterState& setDepthBias(int value) { depthBias = value; return *this; }
        constexpr RasterState& setDepthBiasClamp(float value) { depthBiasClamp = value; return *this; }
        constexpr RasterState& setSlopeScaleDepthBias(float value) { slopeScaledDepthBias = value; return *this; }
        constexpr RasterState& setForcedSampleCount(uint8_t value) { forcedSampleCount = value; return *this; }
        constexpr RasterState& setProgrammableSamplePositionsEnable(bool value) { programmableSamplePositionsEnable = value; return *this; }
        constexpr RasterState& enableProgrammableSamplePositions() { programmableSamplePositionsEnable = true; return *this; }
        constexpr RasterState& disableProgrammableSamplePositions() { programmableSamplePositionsEnable = false; return *this; }
        constexpr RasterState& setConservativeRasterEnable(bool value) { conservativeRasterEnable = value; return *this; }
        constexpr RasterState& enableConservativeRaster() { conservativeRasterEnable = true; return *this; }
        constexpr RasterState& disableConservativeRaster() { conservativeRasterEnable = false; return *this; }
        constexpr RasterState& setQuadFillEnable(bool value) { quadFillEnable = value; return *this; }
        constexpr RasterState& enableQuadFill() { quadFillEnable = true; return *this; }
        constexpr RasterState& disableQuadFill() { quadFillEnable = false; return *this; }
        constexpr RasterState& setSamplePositions(const char* x, const char* y, int count) { for (int i = 0; i < count; i++) { samplePositionsX[i] = x[i]; samplePositionsY[i] = y[i]; } return *this; }
    };


    enum ShaderType : uint16_t
    {

        Invalid = 0,
        VertexShader,
        Pixel,
        Geometry,
        Hull,
        Domain,
        NUM_NON_COMPUTE_SHADER_STAGES,
        Compute = NUM_NON_COMPUTE_SHADER_STAGES,

        NUM_COMPILE_SHADER_STAGES,

        Amplification = NUM_COMPILE_SHADER_STAGES,
        MeshShader,
        TessellationControl,
        TesselationEvaluation,
        AllGraphics,

        RayGeneration,
        AnyHit,
        ClosestHit,
        Miss,
        Intersection ,
        Callable,
        AllRayTracing,

        All,
    };
    NVRHI_ENUM_CLASS_FLAG_OPERATORS(ShaderType)

    enum class VariableShadingRate : uint8_t
    {
        e1x1,
        e1x2,
        e2x1,
        e2x2,
        e2x4,
        e4x2,
        e4x4
    };

    enum class ShadingRateCombiner : uint8_t
    {
        Passthrough,
        Override,
        Min,
        Max,
        ApplyRelative
    };

    struct VariableRateShadingState
    {
        bool enabled = false;
        VariableShadingRate shadingRate = VariableShadingRate::e1x1;
        ShadingRateCombiner pipelinePrimitiveCombiner = ShadingRateCombiner::Passthrough;
        ShadingRateCombiner imageCombiner = ShadingRateCombiner::Passthrough;

        bool operator ==(const VariableRateShadingState& b) const {
            return enabled == b.enabled
                && shadingRate == b.shadingRate
                && pipelinePrimitiveCombiner == b.pipelinePrimitiveCombiner
                && imageCombiner == b.imageCombiner;
        }

        bool operator !=(const VariableRateShadingState& b) const { return !(*this == b); }

        constexpr VariableRateShadingState& setEnabled(bool value) { enabled = value; return *this; }
        constexpr VariableRateShadingState& setShadingRate(VariableShadingRate value) { shadingRate = value; return *this; }
        constexpr VariableRateShadingState& setPipelinePrimitiveCombiner(ShadingRateCombiner value) { pipelinePrimitiveCombiner = value; return *this; }
        constexpr VariableRateShadingState& setImageCombiner(ShadingRateCombiner value) { imageCombiner = value; return *this; }
    };

    struct SinglePassStereoState
    {
        bool enabled = false;
        bool independentViewportMask = false;
        uint16_t renderTargetIndexOffset = 0;

        bool operator ==(const SinglePassStereoState& b) const {
            return enabled == b.enabled
                && independentViewportMask == b.independentViewportMask
                && renderTargetIndexOffset == b.renderTargetIndexOffset;
        }

        bool operator !=(const SinglePassStereoState& b) const { return !(*this == b); }

        constexpr SinglePassStereoState& setEnabled(bool value) { enabled = value; return *this; }
        constexpr SinglePassStereoState& setIndependentViewportMask(bool value) { independentViewportMask = value; return *this; }
        constexpr SinglePassStereoState& setRenderTargetIndexOffset(uint16_t value) { renderTargetIndexOffset = value; return *this; }
    };

 
    enum class CpuAccessMode : uint8_t
    {
        None,
        Read,
        Write
    };

    enum class ResourceStates : uint32_t
    {
        Unknown = 0,
        Common = 0x00000001,
        ConstantBuffer = 0x00000002,
        VertexBuffer = 0x00000004,
        IndexBuffer = 0x00000008,
        IndirectArgument = 0x00000010,
        ShaderResource = 0x00000020,
        UnorderedAccess = 0x00000040,
        RenderTarget = 0x00000080,
        DepthWrite = 0x00000100,
        DepthRead = 0x00000200,
        StreamOut = 0x00000400,
        CopyDest = 0x00000800,
        CopySource = 0x00001000,
        ResolveDest = 0x00002000,
        ResolveSource = 0x00004000,
        Present = 0x00008000,
        AccelStructRead = 0x00010000,
        AccelStructWrite = 0x00020000,
        AccelStructBuildInput = 0x00040000,
        AccelStructBuildBlas = 0x00080000,
        ShadingRateSurface = 0x00100000,
        OpacityMicromapWrite = 0x00200000,
        OpacityMicromapBuildInput = 0x00400000,
    };
    NVRHI_ENUM_CLASS_FLAG_OPERATORS(ResourceStates)
	enum class PrimitiveType : uint8_t
	{
		PointList,
		LineList,
		TriangleList,
		TriangleStrip,
		TriangleFan,
		TriangleListWithAdjacency,
		TriangleStripWithAdjacency,
		PatchList,
        // Topology that defines a screen aligned rectangle N with only 3 vertex corners:
    //    3*N + 0 is upper-left corner,
    //    3*N + 1 is upper-right corner,
    //    3*N + 2 is the lower-left corner.
    // Supported only if GRHISupportsRectTopology == true.
        RectList,
        NUM
	};

    enum class TextureDimension : uint8_t
    {
        Unknown,
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        TextureCube,
        TextureCubeArray,
        Texture2DMS,
        Texture2DMSArray,
        Texture3D,
        RenderBuffer
    };
    // Flags for resources that need to be shared with other graphics APIs or other GPU devices.
    enum class SharedResourceFlags : uint32_t
    {
        None = 0,

        // D3D11: adds D3D11_RESOURCE_MISC_SHARED
        // D3D12: adds D3D12_HEAP_FLAG_SHARED
        // Vulkan: adds vk::ExternalMemoryImageCreateInfo and vk::ExportMemoryAllocateInfo/vk::ExternalMemoryBufferCreateInfo
        Shared = 0x01,

        // D3D11: adds (D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX | D3D11_RESOURCE_MISC_SHARED_NTHANDLE)
        // D3D12, Vulkan: ignored
        Shared_NTHandle = 0x02,

        // D3D12: adds D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER and D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER
        // D3D11, Vulkan: ignored
        Shared_CrossAdapter = 0x04,
    };
    NVRHI_ENUM_CLASS_FLAG_OPERATORS(SharedResourceFlags)
    
        enum class ERHIBindlessSupport : uint8_t
    {
        Unsupported,
        RayTracingOnly,
        AllShaderTypes,

        NumBits = 2
    };

    enum class Format : uint8_t
    {
        UNKNOWN,

        R8_UINT,
        R8_SINT,
        R8_UNORM,
        R8_SNORM,
        RG8_UINT,
        RG8_SINT,
        RG8_UNORM,
        RG8_SNORM,
        R16_UINT,
        R16_SINT,
        R16_UNORM,
        R16_SNORM,
        R16_FLOAT,
        BGRA4_UNORM,
        B5G6R5_UNORM,
        B5G5R5A1_UNORM,
        RGBA8_UINT,
        RGBA8_SINT,
        RGBA8_UNORM,
        RGBA8_SNORM,
        BGRA8_UNORM,
        SRGBA8_UNORM,
        SBGRA8_UNORM,
        R10G10B10A2_UNORM,
        R11G11B10_FLOAT,
        RG16_UINT,
        RG16_SINT,
        RG16_UNORM,
        RG16_SNORM,
        RG16_FLOAT,
        R32_UINT,
        R32_SINT,
        R32_FLOAT,
        RGBA16_UINT,
        RGBA16_SINT,
        RGBA16_FLOAT,
        RGBA16_UNORM,
        RGBA16_SNORM,
        RG32_UINT,
        RG32_SINT,
        RG32_FLOAT,
        RGB32_UINT,
        RGB32_SINT,

        RGB32_FLOAT,
        RGBA32_UINT,
        RGBA32_SINT,
        RGBA32_FLOAT,

        D16,
        D24,
        D24S8, //Depth 24 int Stencil 8 int
        X24G8_UINT,
        D32,   //Ddepth 32 int
        D32_FLOAT, //Depth 32 float
        D32S8,
        X32G8_UINT,

        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC2_UNORM,
        BC2_UNORM_SRGB,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_UNORM,
        BC4_SNORM,
        BC5_UNORM,
        BC5_SNORM,
        BC6H_UFLOAT,
        BC6H_SFLOAT,
        BC7_UNORM,
        BC7_UNORM_SRGB,

        RGB8_UNORM,
        RGB8_FLOAT,
        RGB16_FLOAT,

        COUNT,
    };

    enum class FormatKind : uint8_t
    {
        Integer,
        Normalized,
        Float,
        DepthStencil
    };


    struct FormatInfo
    {
        Format format;
        const char* name;
        uint8_t bytesPerBlock;
        uint8_t blockSize;
        FormatKind kind;
        bool hasRed : 1;
        bool hasGreen : 1;
        bool hasBlue : 1;
        bool hasAlpha : 1;
        bool hasDepth : 1;
        bool hasStencil : 1;
        bool isSigned : 1;
        bool isSRGB : 1;
    };
/**
 *	Resource usage flags - for vertex and index buffers.
 */
	enum class EBufferUsageFlags: int
	{
		None = 0,

		/** The buffer will be written to once. */
		Static = 1 << 0,

		/** The buffer will be written to occasionally, GPU read only, CPU write only.  The data lifetime is until the next update, or the buffer is destroyed. */
		Dynamic = 1 << 1,

		/** The buffer's data will have a lifetime of one frame.  It MUST be written to each frame, or a new one created each frame. */
		Volatile = 1 << 2,

		/** Allows an unordered access view to be created for the buffer. */
		UnorderedAccess = 1 << 3,

		/** Create a byte address buffer, which is basically a structured buffer with a uint32 type. */
		ByteAddressBuffer = 1 << 4,

		/** Buffer that the GPU will use as a source for a copy. */
		SourceCopy = 1 << 5,

		/** Create a buffer that can be bound as a stream output target. */
		StreamOutput = 1 << 6,

		/** Create a buffer which contains the arguments used by DispatchIndirect or DrawIndirect. */
		DrawIndirect = 1 << 7,

		/**
		 * Create a buffer that can be bound as a shader resource.
		 * This is only needed for buffer types which wouldn't ordinarily be used as a shader resource, like a vertex buffer.
		 */
		 ShaderResource = 1 << 8,

		 /** Request that this buffer is directly CPU accessible. */
		 KeepCPUAccessible = 1 << 9,

		 /** Buffer should go in fast vram (hint only). Requires BUF_Transient */
		 FastVRAM = 1 << 10,

		 /** Buffer should be allocated from transient memory. */
		 Transient = None,

		 /** Create a buffer that can be shared with an external RHI or process. */
		 Shared = 1 << 12,

		 /**
		  * Buffer contains opaque ray tracing acceleration structure data.
		  * Resources with this flag can't be bound directly to any shader stage and only can be used with ray tracing APIs.
		  * This flag is mutually exclusive with all other buffer flags except BUF_Static.
		 */
		 AccelerationStructure = 1 << 13,

		 VertexBuffer = 1 << 14,
		 IndexBuffer = 1 << 15,
		 StructuredBuffer = 1 << 16,

		 /** Buffer memory is allocated independently for multiple GPUs, rather than shared via driver aliasing */
		 MultiGPUAllocate = 1 << 17,

		 /**
		  * Tells the render graph to not bother transferring across GPUs in multi-GPU scenarios.  Useful for cases where
		  * a buffer is read back to the CPU (such as streaming request buffers), or written to each frame by CPU (such
		  * as indirect arg buffers), and the other GPU doesn't actually care about the data.
		 */
		 MultiGPUGraphIgnore = 1 << 18,

		 /** Allows buffer to be used as a scratch buffer for building ray tracing acceleration structure,
		  * which implies unordered access. Only changes the buffer alignment and can be combined with other flags.
		 **/
		 RayTracingScratch = (1 << 19) | UnorderedAccess,

		 // Helper bit-masks
		 AnyDynamic = (Dynamic | Volatile),
	};


    //////////////////////////////////////////////////////////////////////////
// Viewport State
//////////////////////////////////////////////////////////////////////////
    struct RHIViewport
    {
        float minX, maxX;
        float minY, maxY;
        float minZ, maxZ;

        RHIViewport() : minX(0.f), maxX(0.f), minY(0.f), maxY(0.f), minZ(0.f), maxZ(1.f) { }

        RHIViewport(float width, float height) : minX(0.f), maxX(width), minY(0.f), maxY(height), minZ(0.f), maxZ(1.f) { }

        RHIViewport(float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ)
            : minX(_minX), maxX(_maxX), minY(_minY), maxY(_maxY), minZ(_minZ), maxZ(_maxZ)
        { }

        bool operator ==(const RHIViewport& b) const
        {
            return minX == b.minX
                && minY == b.minY
                && minZ == b.minZ
                && maxX == b.maxX
                && maxY == b.maxY
                && maxZ == b.maxZ;
        }
        RHIViewport& operator =(const RHIViewport& b)
        {
            if (this == &b) 
                return *this;

            minX = b.minX;
            minY = b.minY;
            minZ = b.minZ;
            maxX = b.maxX;
            maxY = b.maxY;
            maxZ = b.maxZ;
            
            return *this;
        }
        bool operator !=(const RHIViewport& b) const { return !(*this == b); }

        [[nodiscard]] float width() const { return maxX - minX; }
        [[nodiscard]] float height() const { return maxY - minY; }
    };

    struct RHIRect
    {
        int minX, maxX;
        int minY, maxY;

        RHIRect() : minX(0), maxX(0), minY(0), maxY(0) { }
        RHIRect(int width, int height) : minX(0), maxX(width), minY(0), maxY(height) { }
        RHIRect(int _minX, int _maxX, int _minY, int _maxY) : minX(_minX), maxX(_maxX), minY(_minY), maxY(_maxY) { }
        explicit RHIRect(const RHIViewport& viewport)
            : minX(int(floorf(viewport.minX)))
            , maxX(int(ceilf(viewport.maxX)))
            , minY(int(floorf(viewport.minY)))
            , maxY(int(ceilf(viewport.maxY)))
        {
        }

        bool operator ==(const RHIRect& b) const {
            return minX == b.minX && minY == b.minY && maxX == b.maxX && maxY == b.maxY;
        }
        bool operator !=(const RHIRect& b) const { return !(*this == b); }

        [[nodiscard]] int width() const { return maxX - minX; }
        [[nodiscard]] int height() const { return maxY - minY; }
    };


    struct ViewportState
    {
        //These are in pixels
        // note: you can only set each of these either in the PSO or per draw call in DrawArguments
        // it is not legal to have the same state set in both the PSO and DrawArguments
        // leaving these vectors empty means no state is set
        std::vector<RHIViewport> viewports;
        std::vector<RHIRect> scissorRects;

        ViewportState& addViewport(const RHIViewport& v) { viewports.push_back(v); return *this; }
        ViewportState& addScissorRect(const RHIRect& r) { scissorRects.push_back(r); return *this; }
        ViewportState& addViewportAndScissorRect(const RHIViewport& v) { return addViewport(v).addScissorRect(RHIRect(v)); }
    };


    /** Screen Resolution */
    struct RHIScreenResolution
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t RefreshRate;
    };
    //////////////////////////////////////////////////////////////////////////
    // Draw and Dispatch
    //////////////////////////////////////////////////////////////////////////


    struct DrawArguments
    {
        std::vector<uint32_t> indices;
        uint32_t vertexCount = 0; //vertex cnt or index cnt if drawIndex is true
        uint32_t instanceCount = 1;
        uint32_t startIndexLocation = 0;// start from which index
        uint32_t startVertexLocation = 0;// start from which vertex
        uint32_t startInstanceLocation = 0;
        uint32_t indexBufferStride = sizeof(uint32_t);// The size of each index, default uint32_t
        
        bool drawIndex = true;

        constexpr DrawArguments& setVertexCount(uint32_t value) { vertexCount = value; return *this; }
        constexpr DrawArguments& setInstanceCount(uint32_t value) { instanceCount = value; return *this; }
        constexpr DrawArguments& setStartIndexLocation(uint32_t value) { startIndexLocation = value; return *this; }
        constexpr DrawArguments& setStartVertexLocation(uint32_t value) { startVertexLocation = value; return *this; }
        constexpr DrawArguments& setStartInstanceLocation(uint32_t value) { startInstanceLocation = value; return *this; }
    };

    struct DrawIndirectArguments
    {
        uint32_t vertexCount = 0;
        uint32_t instanceCount = 1;
        uint32_t startVertexLocation = 0;
        uint32_t startInstanceLocation = 0;

        constexpr DrawIndirectArguments& setVertexCount(uint32_t value) { vertexCount = value; return *this; }
        constexpr DrawIndirectArguments& setInstanceCount(uint32_t value) { instanceCount = value; return *this; }
        constexpr DrawIndirectArguments& setStartVertexLocation(uint32_t value) { startVertexLocation = value; return *this; }
        constexpr DrawIndirectArguments& setStartInstanceLocation(uint32_t value) { startInstanceLocation = value; return *this; }
    };

    struct DrawIndexedIndirectArguments
    {
        uint32_t indexCount = 0;
        uint32_t instanceCount = 1;
        uint32_t startIndexLocation = 0;
        int32_t  baseVertexLocation = 0;
        uint32_t startInstanceLocation = 0;

        constexpr DrawIndexedIndirectArguments& setIndexCount(uint32_t value) { indexCount = value; return *this; }
        constexpr DrawIndexedIndirectArguments& setInstanceCount(uint32_t value) { instanceCount = value; return *this; }
        constexpr DrawIndexedIndirectArguments& setStartIndexLocation(uint32_t value) { startIndexLocation = value; return *this; }
        constexpr DrawIndexedIndirectArguments& setBaseVertexLocation(int32_t value) { baseVertexLocation = value; return *this; }
        constexpr DrawIndexedIndirectArguments& setStartInstanceLocation(uint32_t value) { startInstanceLocation = value; return *this; }
    };

    struct DispatchRaysArguments
    {
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;

        constexpr DispatchRaysArguments& setWidth(uint32_t value) { width = value; return *this; }
        constexpr DispatchRaysArguments& setHeight(uint32_t value) { height = value; return *this; }
        constexpr DispatchRaysArguments& setDepth(uint32_t value) { depth = value; return *this; }
        constexpr DispatchRaysArguments& setDimensions(uint32_t w, uint32_t h = 1, uint32_t d = 1) { width = w; height = h; depth = d; return *this; }
    };
    
    //////////////////////////////////////////////////////////////////////////
    // RenderTarget
    //////////////////////////////////////////////////////////////////////////

    enum class ERenderTargetStoreAction : uint8_t
    {
        // Contents of the render target emitted during the pass are not stored back to memory.
        ENoAction,

        // Contents of the render target emitted during the pass are stored back to memory.
        EStore,

        // Contents of the render target emitted during the pass are resolved using a box filter and stored back to memory.
        EMultisampleResolve,

        Num,
        NumBits = 2,
    };

    enum class ERenderTargetLoadAction : uint8_t
    {
        // Untouched contents of the render target are undefined. Any existing content is not preserved.
        ENoAction,

        // Existing contents are preserved.
        ELoad,

        // The render target is cleared to the fast clear value specified on the resource.
        EClear,

        Num,
        NumBits = 2,
    };
    static_assert((uint32_t)ERenderTargetLoadAction::Num <= (1 << (uint32_t)ERenderTargetLoadAction::NumBits), "ERenderTargetLoadAction::Num will not fit on ERenderTargetLoadAction::NumBits");

    static_assert((uint32_t)ERenderTargetStoreAction::Num <= (1 << (uint32_t)ERenderTargetStoreAction::NumBits), "ERenderTargetStoreAction::Num will not fit on ERenderTargetStoreAction::NumBits");

    enum class ERenderTargetActions : uint8_t
    {
        LoadOpMask = 2,

#define RTACTION_MAKE_MASK(Load, Store) (((uint8_t)ERenderTargetLoadAction::Load << (uint8_t)LoadOpMask) | (uint8_t)ERenderTargetStoreAction::Store)

        DontLoad_DontStore = RTACTION_MAKE_MASK(ENoAction, ENoAction),

        DontLoad_Store = RTACTION_MAKE_MASK(ENoAction, EStore),
        Clear_Store = RTACTION_MAKE_MASK(EClear, EStore),
        Load_Store = RTACTION_MAKE_MASK(ELoad, EStore),

        Clear_DontStore = RTACTION_MAKE_MASK(EClear, ENoAction),
        Load_DontStore = RTACTION_MAKE_MASK(ELoad, ENoAction),
        Clear_Resolve = RTACTION_MAKE_MASK(EClear, EMultisampleResolve),
        Load_Resolve = RTACTION_MAKE_MASK(ELoad, EMultisampleResolve),

#undef RTACTION_MAKE_MASK
    };

    enum class EDepthStencilTargetActions : uint8_t
    {
        DepthMask = 4,

#define RTACTION_MAKE_MASK(Depth, Stencil) (((uint8_t)ERenderTargetActions::Depth << (uint8_t)DepthMask) | (uint8_t)ERenderTargetActions::Stencil)

        DontLoad_DontStore = RTACTION_MAKE_MASK(DontLoad_DontStore, DontLoad_DontStore),
        DontLoad_StoreDepthStencil = RTACTION_MAKE_MASK(DontLoad_Store, DontLoad_Store),
        DontLoad_StoreStencilNotDepth = RTACTION_MAKE_MASK(DontLoad_DontStore, DontLoad_Store),
        ClearDepthStencil_StoreDepthStencil = RTACTION_MAKE_MASK(Clear_Store, Clear_Store),
        LoadDepthStencil_StoreDepthStencil = RTACTION_MAKE_MASK(Load_Store, Load_Store),
        LoadDepthNotStencil_StoreDepthNotStencil = RTACTION_MAKE_MASK(Load_Store, DontLoad_DontStore),
        LoadDepthNotStencil_DontStore = RTACTION_MAKE_MASK(Load_DontStore, DontLoad_DontStore),
        LoadDepthStencil_StoreStencilNotDepth = RTACTION_MAKE_MASK(Load_DontStore, Load_Store),

        ClearDepthStencil_DontStoreDepthStencil = RTACTION_MAKE_MASK(Clear_DontStore, Clear_DontStore),
        LoadDepthStencil_DontStoreDepthStencil = RTACTION_MAKE_MASK(Load_DontStore, Load_DontStore),
        ClearDepthStencil_StoreDepthNotStencil = RTACTION_MAKE_MASK(Clear_Store, Clear_DontStore),
        ClearDepthStencil_StoreStencilNotDepth = RTACTION_MAKE_MASK(Clear_DontStore, Clear_Store),
        ClearDepthStencil_ResolveDepthNotStencil = RTACTION_MAKE_MASK(Clear_Resolve, Clear_DontStore),
        ClearDepthStencil_ResolveStencilNotDepth = RTACTION_MAKE_MASK(Clear_DontStore, Clear_Resolve),
        LoadDepthClearStencil_StoreDepthStencil = RTACTION_MAKE_MASK(Load_Store, Clear_Store),

        ClearStencilDontLoadDepth_StoreStencilNotDepth = RTACTION_MAKE_MASK(DontLoad_DontStore, Clear_Store),

#undef RTACTION_MAKE_MASK
    };

    enum class SamplerAddressMode : uint8_t
    {
        // D3D names
        Clamp,
        Wrap,
        Border,
        Mirror,
        MirrorOnce,

        // Vulkan names
        ClampToEdge = Clamp,
        Repeat = Wrap,
        ClampToBorder = Border,
        MirroredRepeat = Mirror,
        MirrorClampToEdge = MirrorOnce
    };

    enum class SamplerReductionType : uint8_t
    {
        Standard,
        Comparison,
        Minimum,
        Maximum
    };

    class TextureDesc
    {
    public:
        TextureDesc() {}

        TextureDesc(TextureType _type, const std::string& _path) {
            type = _type;
            path = _path;
            if (type == TextureType::DiffuseMap) {
                format = Format::RGB8_UNORM;
            }
            else if (type == TextureType::AoMap) {
                format = Format::RGBA8_UNORM;
            }
            else if (type == TextureType::RoughnessMap) {
                format = Format::RGBA8_UNORM;

            }
            else if (type == TextureType::MentallicMap) {
                format = Format::R8_UNORM;

            }
            else if (type == TextureType::NormalMap) {
                format = Format::RGB8_UNORM;
            }
            else if (type == TextureType::EmissionMap) {
                format = Format::RGB8_UNORM;
            }
            else {
                GE_CORE_WARN("Unknown texture type\n");
            }
        }
        TextureType type = TextureType::None;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        uint32_t arraySize = 1;
        uint32_t mipLevelsCnt = 1;
        uint32_t defaultMipLevel = 0;

        uint32_t sampleCount = 1;
        uint32_t sampleQuality = 0;
        Format format = Format::UNKNOWN;
        TextureDimension dimension = TextureDimension::Texture2D;
        TextureAccess access = TextureAccess::ReadOnly;
        std::string debugName;

        /** Clear value to use when fast-clearing the texture. */
        //TODO::
        //FClearValueBinding ClearValue;

        bool isLayer = false; //for texture array
        uint32_t layer = 0;           //texure index in texture array

        bool isShaderResource = true; // Note: isShaderResource is initialized to 'true' for backward compatibility
        bool isRenderTarget = false;
        bool isUAV = false;
        bool isTypeless = false;
        bool isShadingRateSurface = false;
        bool isExternalTexture = false;

        SharedResourceFlags sharedResourceFlags = SharedResourceFlags::None;

        // Indicates that the texture is created with no backing memory,
        // and memory is bound to the texture later using bindTextureMemory.
        // On DX12, the texture resource is created at the time of memory binding.
        bool isVirtual = false;

        Color clearValue;
        bool useClearValue = false;

        ResourceStates initialState = ResourceStates::Unknown;

        // If keepInitialState is true, command lists that use the texture will automatically
        // begin tracking the texture from the initial state and transition it to the initial state 
        // on command list close.
        bool keepInitialState = false;

        // opengl 不需要sampler， filter在texture desc 里配置
        FilterMode minFilter = FilterMode::Linear;
        FilterMode magFilter = FilterMode::Linear;
        FilterMode mipFilter = FilterMode::Linear_Mip_Linear;


        constexpr bool isMultiSample() const { return sampleCount > 1; }





        //int internalFormat = GL_RGBA;
        //int glformat = GL_RGBA;
        //TODO:: split u, v, w warp
        SamplerAddressMode wrap = SamplerAddressMode::ClampToEdge;
        //unsigned int dataType = GL_UNSIGNED_BYTE;
        bool generateMipmap = false;
        std::string path;
        //for cubemap
        std::vector<std::string> faces;
        float* data = nullptr;

        constexpr TextureDesc& setWidth(uint32_t value) { width = value; return *this; }
        constexpr TextureDesc& setHeight(uint32_t value) { height = value; return *this; }
        constexpr TextureDesc& setDepth(uint32_t value) { depth = value; return *this; }
        constexpr TextureDesc& setArraySize(uint32_t value) { arraySize = value; return *this; }
        constexpr TextureDesc& setMipLevels(uint32_t value) { mipLevelsCnt = value; return *this; }
        constexpr TextureDesc& setSampleCount(uint32_t value) { sampleCount = value; return *this; }
        constexpr TextureDesc& setSampleQuality(uint32_t value) { sampleQuality = value; return *this; }
        constexpr TextureDesc& setFormat(Format value) { format = value; return *this; }
        constexpr TextureDesc& setDimension(TextureDimension value) { dimension = value; return *this; }
        TextureDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
        constexpr TextureDesc& setIsRenderTarget(bool value) { isRenderTarget = value; return *this; }
        constexpr TextureDesc& setIsUAV(bool value) { isUAV = value; return *this; }
        constexpr TextureDesc& setIsTypeless(bool value) { isTypeless = value; return *this; }
        constexpr TextureDesc& setIsVirtual(bool value) { isVirtual = value; return *this; }
        constexpr TextureDesc& setClearValue(const Color& value) { clearValue = value; useClearValue = true; return *this; }
        constexpr TextureDesc& setUseClearValue(bool value) { useClearValue = value; return *this; }
        constexpr TextureDesc& setInitialState(ResourceStates value) { initialState = value; return *this; }
        constexpr TextureDesc& setKeepInitialState(bool value) { keepInitialState = value; return *this; }
    };
    class FExclusiveDepthStencil
    {
    public:
        enum Type
        {
            // don't use those directly, use the combined versions below
            // 4 bits are used for depth and 4 for stencil to make the hex value readable and non overlapping
            DepthNop = 0x00,
            DepthRead = 0x01,
            DepthWrite = 0x02,
            DepthMask = 0x0f,
            StencilNop = 0x00,
            StencilRead = 0x10,
            StencilWrite = 0x20,
            StencilMask = 0xf0,

            // use those:
            DepthNop_StencilNop = DepthNop + StencilNop,
            DepthRead_StencilNop = DepthRead + StencilNop,
            DepthWrite_StencilNop = DepthWrite + StencilNop,
            DepthNop_StencilRead = DepthNop + StencilRead,
            DepthRead_StencilRead = DepthRead + StencilRead,
            DepthWrite_StencilRead = DepthWrite + StencilRead,
            DepthNop_StencilWrite = DepthNop + StencilWrite,
            DepthRead_StencilWrite = DepthRead + StencilWrite,
            DepthWrite_StencilWrite = DepthWrite + StencilWrite,
        };

    private:
        Type Value;

    public:
        // constructor
        FExclusiveDepthStencil(Type InValue = DepthNop_StencilNop)
            : Value(InValue)
        {
        }

        inline bool IsUsingDepthStencil() const
        {
            return Value != DepthNop_StencilNop;
        }
        inline bool IsUsingDepth() const
        {
            return (ExtractDepth() != DepthNop);
        }
        inline bool IsUsingStencil() const
        {
            return (ExtractStencil() != StencilNop);
        }
        inline bool IsDepthWrite() const
        {
            return ExtractDepth() == DepthWrite;
        }
        inline bool IsDepthRead() const
        {
            return ExtractDepth() == DepthRead;
        }
        inline bool IsStencilWrite() const
        {
            return ExtractStencil() == StencilWrite;
        }
        inline bool IsStencilRead() const
        {
            return ExtractStencil() == StencilRead;
        }

        inline bool IsAnyWrite() const
        {
            return IsDepthWrite() || IsStencilWrite();
        }

        inline void SetDepthWrite()
        {
            Value = (Type)(ExtractStencil() | DepthWrite);
        }
        inline void SetStencilWrite()
        {
            Value = (Type)(ExtractDepth() | StencilWrite);
        }
        inline void SetDepthStencilWrite(bool bDepth, bool bStencil)
        {
            Value = DepthNop_StencilNop;

            if (bDepth)
            {
                SetDepthWrite();
            }
            if (bStencil)
            {
                SetStencilWrite();
            }
        }
        bool operator==(const FExclusiveDepthStencil& rhs) const
        {
            return Value == rhs.Value;
        }

        bool operator != (const FExclusiveDepthStencil& RHS) const
        {
            return Value != RHS.Value;
        }

        inline bool IsValid(FExclusiveDepthStencil& Current) const
        {
            Type Depth = ExtractDepth();

            if (Depth != DepthNop && Depth != Current.ExtractDepth())
            {
                return false;
            }

            Type Stencil = ExtractStencil();

            if (Stencil != StencilNop && Stencil != Current.ExtractStencil())
            {
                return false;
            }

            return true;
        }

    private:
        inline Type ExtractDepth() const
        {
            return (Type)(Value & DepthMask);
        }
        inline Type ExtractStencil() const
        {
            return (Type)(Value & StencilMask);
        }
    };


    // Hints for some RHIs that support subpasses
    enum class ESubpassHint : uint8_t
    {
        // Regular rendering
        None,

        // Render pass has depth reading subpass
        DepthReadSubpass,

        // Mobile defferred shading subpass
        DeferredShadingSubpass,
    };
    enum EVRSRateCombiner : uint8_t
    {
        VRSRB_Passthrough,
        VRSRB_Override,
        VRSRB_Min,
        VRSRB_Max,
        VRSRB_Sum,
    };





  
    inline ERenderTargetActions MakeRenderTargetActions(ERenderTargetLoadAction Load, ERenderTargetStoreAction Store)
    {
        return (ERenderTargetActions)(((uint8_t)Load << (uint8_t)ERenderTargetActions::LoadOpMask) | (uint8_t)Store);
    }

    inline ERenderTargetLoadAction GetLoadAction(ERenderTargetActions Action)
    {
        return (ERenderTargetLoadAction)((uint8_t)Action >> (uint8_t)ERenderTargetActions::LoadOpMask);
    }

    inline ERenderTargetStoreAction GetStoreAction(ERenderTargetActions Action)
    {
        return (ERenderTargetStoreAction)((uint8_t)Action & ((1 << (uint8_t)ERenderTargetActions::LoadOpMask) - 1));
    }

    inline constexpr EDepthStencilTargetActions MakeDepthStencilTargetActions(const ERenderTargetActions Depth, const ERenderTargetActions Stencil)
    {
        return (EDepthStencilTargetActions)(((uint8_t)Depth << (uint8_t)EDepthStencilTargetActions::DepthMask) | (uint8_t)Stencil);
    }

    inline ERenderTargetActions GetDepthActions(EDepthStencilTargetActions Action)
    {
        return (ERenderTargetActions)((uint8_t)Action >> (uint8_t)EDepthStencilTargetActions::DepthMask);
    }

    inline ERenderTargetActions GetStencilActions(EDepthStencilTargetActions Action)
    {
        return (ERenderTargetActions)((uint8_t)Action & ((1 << (uint8_t)EDepthStencilTargetActions::DepthMask) - 1));
    }



    template <class T>
    void hash_combine(size_t& seed, const T& v)
    {
     /*   std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);*/
    }
}
