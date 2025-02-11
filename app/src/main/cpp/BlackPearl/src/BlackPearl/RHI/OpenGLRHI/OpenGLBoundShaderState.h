#pragma once
#include "BlackPearl/RHI/RHIBoundShaderState.h"
#include "BlackPearl/RHI/RHIShader.h"
#include "OpenGLShader.h"
namespace BlackPearl {
	
	class FOpenGLLinkedProgram;

	class BoundShaderState : public RefCounter<IBoundShaderState>
	{
	public:
		BoundShaderState(
			IInputLayout* InVertexDeclarationRHI
			, IShader* VertexShader
			, IShader* PixelShader
			, IShader* GeometryShader
		) /*: IBoundShaderState(
			InVertexDeclarationRHI
			, VertexShader
			, PixelShader
			, GeometryShader*/

		{
			VertexDeclarationRHI = InVertexDeclarationRHI;
			VertexShaderRHI = VertexShader;
			PixelShaderRHI = PixelShader;
#if PLATFORM_SUPPORTS_GEOMETRY_SHADERS
			GeometryShaderRHI = GeometryShader;
#endif
		}
		//uint16_t StreamStrides[MaxVertexElementCount];

		FOpenGLLinkedProgram* LinkedProgram;
		//TRefCountPtr<FOpenGLVertexDeclaration> VertexDeclaration;
	

	private:

	};

}

