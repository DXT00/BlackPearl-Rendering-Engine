#pragma once
#include "BlackPearl/RHI/RHIShader.h"
#include "BlackPearl\RHI\RHIBoundShaderState.h"
#include "BlackPearl\Core\Container\TBitArray.h"
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
	
		const TBitArray& GetTextureNeeds(int32_t& OutMaxTextureStageUsed);
		const TBitArray& GetUAVNeeds(int32_t& OutMaxUAVUnitUsed) const;
	private:

	};

}

