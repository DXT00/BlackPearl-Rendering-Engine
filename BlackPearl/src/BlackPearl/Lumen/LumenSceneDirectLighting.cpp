#include "pch.h"
#include "LumenSceneDirectLighting.h"
namespace BlackPearl {

	/*class FSpliceCardPagesIntoTilesCS
	{
		
			RDG_BUFFER_ACCESS(IndirectArgBuffer, ERHIAccess::IndirectArgs)
			SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
			SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FLumenCardScene, LumenCardScene)
			SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FLumenPackedLight>, LumenPackedLights)
			SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint>, RWCardTileAllocator)
			SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint>, RWCardTiles)
			SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint>, RWLightTileAllocatorPerLight)
			SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint>, CardPageIndexAllocator)
			SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint>, CardPageIndexData)
			uint32_t MaxLightsPerTile;
			uint32_t NumLights;

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
		{
			return DoesPlatformSupportLumenGI(Parameters.Platform);
		}

		static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
		{
			FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
			OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZE"), GetGroupSize());
		}

		static int32_t GetGroupSize()
		{
			return 8;
		}
	};

	IMPLEMENT_GLOBAL_SHADER(FSpliceCardPagesIntoTilesCS, "/Engine/Private/Lumen/LumenSceneDirectLightingCulling.usf", "SpliceCardPagesIntoTilesCS", SF_Compute);*/

}
