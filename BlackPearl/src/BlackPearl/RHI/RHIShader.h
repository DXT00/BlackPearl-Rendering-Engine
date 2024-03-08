#pragma once
#include "RHIBindingSet.h"
namespace BlackPearl {

class IPipeline;

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

    ShaderDesc() = default;

    ShaderDesc(ShaderType type)
        : shaderType(type)
    { }
};

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
    virtual IPipeline* getPipeline() = 0;
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