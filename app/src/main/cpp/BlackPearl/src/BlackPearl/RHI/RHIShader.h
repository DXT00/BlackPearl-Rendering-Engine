#pragma once
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


}