#pragma once
#include <string>
#include "stdint.h"
#include "RHIBindingSet.h"
namespace BlackPearl {

    /** @warning: update *LegacyShaderPlatform* when the below changes */
    enum EShaderPlatform : uint16_t
    {
        SP_PCD3D_SM5 = 0,
        SP_PCD3D_ES3_1 = 14,
        SP_OPENGL_PCES3_1 = 15,
        SP_VULKAN_PCES3_1 = 17,
        SP_VULKAN_SM5 = 20,
        SP_VULKAN_ES3_1_ANDROID = 21,
        SP_OPENGL_ES3_1_ANDROID = 24,
        SP_StaticPlatform_First = 32,
#define DDPI_NUM_STATIC_SHADER_PLATFORMS 16
        SP_StaticPlatform_Last = (SP_StaticPlatform_First + DDPI_NUM_STATIC_SHADER_PLATFORMS - 1),

        //  Add new platforms below this line, starting from (SP_StaticPlatform_Last + 1)
        //---------------------------------------------------------------------------------
        SP_VULKAN_SM5_ANDROID = SP_StaticPlatform_Last + 1,
        SP_PCD3D_SM6 = SP_StaticPlatform_Last + 2,
        SP_VULKAN_SM6 = SP_StaticPlatform_Last + 4,

        SP_NumPlatforms,
        SP_NumBits = 16,
    };
    /**
 * The RHI's feature level indicates what level of support can be relied upon.
 * Note: these are named after graphics API's like ES3 but a feature level can be used with a different API (eg ERHIFeatureLevel::ES3.1 on D3D11)
 * As long as the graphics API supports all the features of the feature level (eg no ERHIFeatureLevel::SM5 on OpenGL ES3.1)
 */
    namespace ERHIFeatureLevel
    {
        enum Type : int
        {

            /** Feature level defined by the core capabilities of OpenGL ES3.1 & Metal/Vulkan. */
            ES3_1,
            /**
             * Feature level defined by the capabilities of DX11 Shader Model 5.
             *   Compute shaders with shared memory, group sync, UAV writes, integer atomics
             *   Indirect drawing
             *   Pixel shaders with UAV writes
             *   Cubemap arrays
             *   Read-only depth or stencil views (eg read depth buffer as SRV while depth test and stencil write)
             * Tessellation is not considered part of Feature Level SM5 and has a separate capability flag.
             */
            SM5,

            /**
             * Feature level defined by the capabilities of DirectX 12 hardware feature level 12_2 with Shader Model 6.5
             *   Raytracing Tier 1.1
             *   Mesh and Amplification shaders
             *   Variable rate shading
             *   Sampler feedback
             *   Resource binding tier 3
             */
            SM6,

            Num
        };
    };
struct ShaderSpecialization
{
    uint32_t constantID = 0;
    union
    {
        uint32_t u = 0;
        int32_t i;
        float f;
    } value;

    static ShaderSpecialization UInt32(uint32_t constantID, uint32_t u) {
        ShaderSpecialization s;
        s.constantID = constantID;
        s.value.u = u;
        return s;
    }

    static ShaderSpecialization Int32(uint32_t constantID, int32_t i) {
        ShaderSpecialization s;
        s.constantID = constantID;
        s.value.i = i;
        return s;
    }

    static ShaderSpecialization Float(uint32_t constantID, float f) {
        ShaderSpecialization s;
        s.constantID = constantID;
        s.value.f = f;
        return s;
    }
};

struct CustomSemantic
{
    enum Type
    {
        Undefined = 0,
        XRight = 1,
        ViewportMask = 2
    };

    Type type;
    std::string name;
};
struct ShaderDesc
{
    ShaderType shaderType = ShaderType::None;
    std::string debugName;
    std::string entryName = "main";
    //glsl file
    std::string filePath;

    int hlslExtensionsUAV = -1;

    bool useSpecificShaderExt = false;
    uint32_t numCustomSemantics = 0;
    CustomSemantic* pCustomSemantics = nullptr;

    uint32_t* pCoordinateSwizzling = nullptr;
    //for opengl 
    uint32_t binaryformat = 0;
    ShaderDesc() = default;

    ShaderDesc(ShaderType type)
        : shaderType(type)
    { }
};
//for raytracing
class IRayTracingPipeline;
class IShaderTable :public IResource
{
public:
    virtual void setRayGenerationShader(const char* exportName, IBindingSet* bindings = nullptr) = 0;
    virtual int addMissShader(const char* exportName, IBindingSet* bindings = nullptr) = 0;
    virtual int addHitGroup(const char* exportName, IBindingSet* bindings = nullptr) = 0;
    virtual int addCallableShader(const char* exportName, IBindingSet* bindings = nullptr) = 0;
    virtual void clearMissShaders() = 0;
    virtual void clearHitShaders() = 0;
    virtual void clearCallableShaders() = 0;
    virtual IRayTracingPipeline* getPipeline() = 0;
};
typedef RefCountPtr<IShaderTable> ShaderTableHandle;

class IShader : public IResource
{
public:
    [[nodiscard]] virtual const ShaderDesc& getDesc() const = 0;
    virtual void getBytecode(const void** ppBytecode, size_t* pSize) const = 0;
};
typedef RefCountPtr<IShader> ShaderHandle;


class ShaderParameters
{
public:
    std::vector<IBindingLayout*> bindingLayouts;
    std::vector<IBindingSet*> bindingSets;


};
namespace RHICore {


    //    template <typename TResourceType, typename TCallback>
    //    inline void EnumerateUniformBufferResources(FRHIUniformBuffer* RESTRICT Buffer, int32_t BufferIndex, const uint32_t* RESTRICT ResourceMap, TCallback&& Callback)
    //    {
    //        const TRefCountPtr<FRHIResource>* RESTRICT Resources = Buffer->GetResourceTable().GetData();
    //
    //        uint32_t BufferOffset = ResourceMap[BufferIndex];
    //        if (BufferOffset > 0)
    //        {
    //            const uint32_t* RESTRICT ResourceInfos = &ResourceMap[BufferOffset];
    //            uint32_t ResourceInfo = *ResourceInfos++;
    //            do
    //            {
    //                checkSlow(FRHIResourceTableEntry::GetUniformBufferIndex(ResourceInfo) == BufferIndex);
    //
    //                const uint16 ResourceIndex = FRHIResourceTableEntry::GetResourceIndex(ResourceInfo);
    //                const uint8  BindIndex = FRHIResourceTableEntry::GetBindIndex(ResourceInfo);
    //
    //                TResourceType* Resource = static_cast<TResourceType*>(Resources[ResourceIndex].GetReference());
    //                checkf(Resource
    //                    , TEXT("Null %s (resource %d bind %d) on UB Layout %s")
    //                    , TResourceTypeStr<TResourceType>::String
    //                    , ResourceIndex
    //                    , BindIndex
    //                    , *Buffer->GetLayout().GetDebugName()
    //                );
    //
    //                Callback(Resource, BindIndex);
    //
    //                ResourceInfo = *ResourceInfos++;
    //            } while (FRHIResourceTableEntry::GetUniformBufferIndex(ResourceInfo) == BufferIndex);
    //        }
    //    }
    //
    //
    //    template <typename TBinder, typename TUniformBufferArrayType, typename TBitMaskType>
    //    void SetResourcesFromTables(TBinder&& Binder, IShader const& Shader, FShaderResourceTable const& SRT, TBitMaskType& DirtyUniformBuffers, TUniformBufferArrayType const& BoundUniformBuffers
    //#if ENABLE_RHI_VALIDATION
    //        , RHIValidation::FTracker* Tracker
    //#endif
    //    )
    //    {
    //        float CurrentTimeForTextureTimes = FApp::GetCurrentTime();
    //
    //        // Mask the dirty bits by those buffers from which the shader has bound resources.
    //        uint32_t DirtyBits = SRT.ResourceTableBits & DirtyUniformBuffers;
    //        while (DirtyBits)
    //        {
    //            // Scan for the lowest set bit, compute its index, clear it in the set of dirty bits.
    //            const uint32_t LowestBitMask = (DirtyBits) & (-(int32_t)DirtyBits);
    //            const int32_t BufferIndex = FMath::CountTrailingZeros(LowestBitMask); // todo: This has a branch on zero, we know it could never be zero...
    //            DirtyBits ^= LowestBitMask;
    //
    //            check(BufferIndex < SRT.ResourceTableLayoutHashes.Num());
    //
    //            FRHIUniformBuffer* Buffer = BoundUniformBuffers[BufferIndex];
    //
    //#if DO_CHECK
    //
    //            if (!Buffer)
    //            {
    //                UE_LOG(LogRHICore, Fatal, TEXT("Shader expected a uniform buffer at slot %u but got null instead (Shader='%s' UB='%s'). Rendering code needs to set a valid uniform buffer for this slot.")
    //                    , BufferIndex
    //                    , Shader.GetShaderName()
    //                    , *Shader.GetUniformBufferName(BufferIndex)
    //                );
    //            }
    //            else if (Buffer->GetLayout().GetHash() != SRT.ResourceTableLayoutHashes[BufferIndex])
    //            {
    //                FRHIUniformBufferLayout const& BufferLayout = Buffer->GetLayout();
    //
    //                FString ResourcesString;
    //                for (FRHIUniformBufferResource const& Resource : BufferLayout.Resources)
    //                {
    //                    ResourcesString += FString::Printf(TEXT("%s%d")
    //                        , ResourcesString.Len() ? TEXT(" ") : TEXT("")
    //                        , Resource.MemberType
    //                    );
    //                }
    //
    //                // This might mean you are accessing a data you haven't bound e.g. GBuffer
    //                UE_LOG(LogRHICore, Fatal,
    //                    TEXT("Uniform buffer bound to slot %u is not what the shader expected:\n")
    //                    TEXT("\tBound                : Uniform Buffer[%s] with Hash[0x%08x]\n")
    //                    TEXT("\tExpected             : Uniform Buffer[%s] with Hash[0x%08x]\n")
    //                    TEXT("\tShader Name          : %s\n")
    //                    TEXT("\tLayout CB Size       : %d\n")
    //                    TEXT("\tLayout Num Resources : %d\n")
    //                    TEXT("\tResource Types       : %s\n")
    //                    , BufferIndex
    //                    , *BufferLayout.GetDebugName(), BufferLayout.GetHash()
    //                    , *Shader.GetUniformBufferName(BufferIndex), SRT.ResourceTableLayoutHashes[BufferIndex]
    //                    , Shader.GetShaderName()
    //                    , BufferLayout.ConstantBufferSize
    //                    , BufferLayout.Resources.Num()
    //                    , *ResourcesString
    //                );
    //            }
    //
    //#endif // DO_CHECK
    //
    //            // Textures
    //            EnumerateUniformBufferResources<FRHITexture>(Buffer, BufferIndex, SRT.TextureMap.GetData(),
    //                [&](FRHITexture* Texture, uint8 Index)
    //                {
    //#if ENABLE_RHI_VALIDATION
    //                    if (Tracker)
    //                    {
    //                        ERHIAccess Access = IsComputeShaderFrequency(Shader.GetFrequency())
    //                            ? ERHIAccess::SRVCompute
    //                            : ERHIAccess::SRVGraphics;
    //
    //                        // Textures bound here only have their "common" plane accessible. Stencil etc is ignored.
    //                        // (i.e. only access the color plane of a color texture, or depth plane of a depth texture)
    //                        Tracker->Assert(Texture->GetViewIdentity(0, 0, 0, 0, uint32_t(RHIValidation::EResourcePlane::Common), 1), Access);
    //                    }
    //#endif
    //                    Texture->SetLastRenderTime(CurrentTimeForTextureTimes);
    //                    Binder.SetTexture(Texture, Index);
    //                });
    //
    //            // SRVs
    //            EnumerateUniformBufferResources<FRHIShaderResourceView>(Buffer, BufferIndex, SRT.ShaderResourceViewMap.GetData(),
    //                [&](FRHIShaderResourceView* SRV, uint8 Index)
    //                {
    //#if ENABLE_RHI_VALIDATION
    //                    if (Tracker)
    //                    {
    //                        ERHIAccess Access = IsComputeShaderFrequency(Shader.GetFrequency())
    //                            ? ERHIAccess::SRVCompute
    //                            : ERHIAccess::SRVGraphics;
    //
    //                        Tracker->Assert(SRV->GetViewIdentity(), Access);
    //                    }
    //                    if (GRHIValidationEnabled)
    //                    {
    //                        RHIValidation::ValidateShaderResourceView(&Shader, Index, SRV);
    //                    }
    //#endif
    //                    Binder.SetSRV(SRV, Index);
    //                });
    //
    //            // Samplers
    //            EnumerateUniformBufferResources<FRHISamplerState>(Buffer, BufferIndex, SRT.SamplerMap.GetData(),
    //                [&](FRHISamplerState* Sampler, uint8 Index)
    //                {
    //                    Binder.SetSampler(Sampler, Index);
    //                });
    //
    //            // UAVs
    //            EnumerateUniformBufferResources<FRHIUnorderedAccessView>(Buffer, BufferIndex, SRT.UnorderedAccessViewMap.GetData(),
    //                [&](FRHIUnorderedAccessView* UAV, uint8 Index)
    //                {
    //#if ENABLE_RHI_VALIDATION
    //                    if (Tracker)
    //                    {
    //                        ERHIAccess Access = IsComputeShaderFrequency(Shader.GetFrequency())
    //                            ? ERHIAccess::UAVCompute
    //                            : ERHIAccess::UAVGraphics;
    //
    //                        Tracker->AssertUAV(UAV, Access, Index);
    //                    }
    //#endif
    //                    Binder.SetUAV(UAV, Index);
    //                });
    //        }
    //
    //        DirtyUniformBuffers = TBitMaskType(0);
    //    }
    //
    //
    //}
}


}



