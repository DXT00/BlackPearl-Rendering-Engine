#pragma once
#include "BlackPearl/RHI/RHIShader.h"
#include "BlackPearl\RHI\RHIBoundShaderState.h"
#include "BlackPearl\Core\Container\TBitArray.h"
#include "OpenGLShader.h"
#include "OpenGLBindingSet.h"
namespace BlackPearl {
	
	class FOpenGLLinkedProgram;

    /*
    
        for (size_t i = 0; i < bindingSet.size(); i++)
        {
            BindingSet* bs = dynamic_cast<BindingSet*>(bindingSet[i]);
            Config.bindingSet.push_back(bs);
        }

    */
	class BoundShaderState : public RefCounter<IBoundShaderState>
	{
	public:
		BoundShaderState(
			IInputLayout* InVertexDeclarationRHI
			, IShader* VertexShader
			, IShader* PixelShader
			, IShader* GeometryShader,
			FOpenGLLinkedProgram* _LinkedProgram,
            const std::vector<IBindingSet*>& bindingSet
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
			LinkedProgram = _LinkedProgram;

            for (size_t i = 0; i < bindingSet.size(); i++)
            {
                BindingSet* bs = dynamic_cast<BindingSet*>(bindingSet[i]);
                BindingSets.push_back(bs);
            }

		}

		FOpenGLLinkedProgram* LinkedProgram;
        std::vector<BindingSet*> BindingSets;

		//TRefCountPtr<FOpenGLVertexDeclaration> VertexDeclaration;
	
		const TBitArray& GetTextureNeeds(int32_t& OutMaxTextureStageUsed);
		const TBitArray& GetUAVNeeds(int32_t& OutMaxUAVUnitUsed) const;
	private:

	};

}

