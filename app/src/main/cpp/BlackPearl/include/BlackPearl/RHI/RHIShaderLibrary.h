#pragma once
#include "RHIResources.h"
#include "RHIShader.h"
//////////////////////////////////////////////////////////////////////////
 // Shader Library
 //////////////////////////////////////////////////////////////////////////
namespace BlackPearl {
    class IShaderLibrary : public IResource
    {
    public:
        virtual void getBytecode(const void** ppBytecode, size_t* pSize) const = 0;
        virtual ShaderHandle getShader(const char* entryName, ShaderType shaderType) = 0;
    };

    typedef RefCountPtr<IShaderLibrary> ShaderLibraryHandle;

}
