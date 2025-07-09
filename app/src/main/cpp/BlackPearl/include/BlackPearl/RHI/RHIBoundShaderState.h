#pragma once
#include "RefCountPtr.h"
#include "RHIResources.h"
#include "RHIInputLayout.h"
#include "RHIShader.h"
namespace BlackPearl{


    class IBoundShaderState:  public IResource
    {
    public:
        IBoundShaderState() {}

        IBoundShaderState
        (
            IInputLayout* InVertexDeclarationRHI
            , IShader* InVertexShaderRHI
            , IShader* InPixelShaderRHI
            , IShader* InGeometryShaderRHI
            , IShader* InComputeShaderRHI
        )
            : VertexDeclarationRHI(InVertexDeclarationRHI)
            , VertexShaderRHI(InVertexShaderRHI)
            , PixelShaderRHI(InPixelShaderRHI)
            , ComputeShaderRHI(InComputeShaderRHI)
#if PLATFORM_SUPPORTS_GEOMETRY_SHADERS
            , GeometryShaderRHI(InGeometryShaderRHI)
#endif
        {
        }

#if PLATFORM_SUPPORTS_MESH_SHADERS
        inline FBoundShaderStateInput(
            IShader* InMeshShaderRHI,
            IShader* InAmplificationShader,
            IShader* InPixelShaderRHI)
            : PixelShaderRHI(InPixelShaderRHI)
            , MeshShaderRHI(InMeshShaderRHI)
            , AmplificationShaderRHI(InAmplificationShader)
        {
        }
#endif
        IShader* GetVertexShader() const { return VertexShaderRHI; }
        IShader* GetPixelShader() const { return PixelShaderRHI; }
        IShader* GetComputeShader() const { return ComputeShaderRHI; }

#if PLATFORM_SUPPORTS_MESH_SHADERS
        IShader* GetMeshShader() const { return MeshShaderRHI; }
        void SetMeshShader(IShader* InMeshShader) { MeshShaderRHI = InMeshShader; }
        IShader* GetAmplificationShader() const { return AmplificationShaderRHI; }
        void SetAmplificationShader(IShader* InAmplificationShader) { AmplificationShaderRHI = InAmplificationShader; }
#else
        IShader* GetMeshShader() const { return nullptr; }
        void SetMeshShader(IShader*) {}
        IShader* GetAmplificationShader() const { return nullptr; }
        void SetAmplificationShader(IShader*) {}
#endif

#if PLATFORM_SUPPORTS_GEOMETRY_SHADERS
        IShader* GetGeometryShader() const { return GeometryShaderRHI; }
        void SetGeometryShader(IShader* InGeometryShader) { GeometryShaderRHI = InGeometryShader; }
#else
        IShader* GetGeometryShader() const { return nullptr; }
        void SetGeometryShader(IShader*) {}
#endif
        IInputLayout* GetInputLayout() const { return VertexDeclarationRHI; }
    
    protected:
        IInputLayout* VertexDeclarationRHI = nullptr;
        IShader* VertexShaderRHI = nullptr;
        IShader* PixelShaderRHI = nullptr;
        IShader* ComputeShaderRHI = nullptr;

#if PLATFORM_SUPPORTS_MESH_SHADERS
        IShader* MeshShaderRHI = nullptr;
        IShader* AmplificationShaderRHI = nullptr;
#endif
#if PLATFORM_SUPPORTS_GEOMETRY_SHADERS
        IShader* GeometryShaderRHI = nullptr;
#endif
    };
    //typedef RefCountPtr<IBoundShaderState> BoundShaderStateHandle;
}
