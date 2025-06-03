#include "pch.h"
//#include <glad/glad.h>
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"

#include "Config.h"
#include "BlackPearl/RHI/DynamicRHI.h"

namespace BlackPearl {

    int32_t Configuration::SyncInterval = 0;

	void Configuration::SyncGPU()
	{
		GLsync sync_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		GLenum wait_return = GL_UNSIGNALED;
		while (wait_return != GL_ALREADY_SIGNALED && wait_return != GL_CONDITION_SATISFIED)
			wait_return = glClientWaitSync(sync_fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
		glDeleteSync(sync_fence);
	}
	const char* Configuration::GetShaderTypeName()
	{
		switch (DynamicRHI::g_RHIType)
		{

		case DynamicRHI::Type::D3D12:
			return "dxbc";
		case DynamicRHI::Type::Vulkan:
			return "spv";
		case DynamicRHI::Type::OpenGL:
			assert(!"no need to compile shaders externally");
			return "";
		default:
			assert(!"Unknown graphics API");
			return "";
		}
	}
	//const VoxelConeTracingRenderer::RenderingMode Configuration::RenderingMode = VoxelConeTracingRenderer::RenderingMode::VOXEL_CONE_TRACING;
	const float Configuration::ShadowMapPointLightRadius = 30.0f;

    /* prefilterMap.glsl里的	float resolution =512.0;也要改 */
	const float Configuration::EnvironmantMapResolution = 64.0;// 256.0f;
	
    const bool Configuration::bDeferredShading = true;

    const bool Configuration::bUseSinglePass = false;


	const char* ShaderConfig::AMBIENT_COLOR   = "u_Material.ambientColor";
	const char* ShaderConfig::DIFFUSE_COLOR   = "u_Material.diffuseColor";
	const char* ShaderConfig::SPECULAR_COLOR  = "u_Material.specularColor";
	const char* ShaderConfig::EMISSION_COLOR  = "u_Material.emissiveColor";
	const char* ShaderConfig::METALLIC_VALUE  = "u_Material.metalnessValue";
	const char* ShaderConfig::ROUGHNESS_VALUE = "u_Material.roughnessValue";
	const char* ShaderConfig::AO_VALUE        = "u_Material.aoValue";

	const char* ShaderConfig::DIFFUSE_TEXTURE2D   = "u_Material.diffuse";
	const char* ShaderConfig::SPECULAR_TEXTURE2D  = "u_Material.specular";
	const char* ShaderConfig::EMISSION_TEXTURE2D  = "u_Material.emission";
	const char* ShaderConfig::NORMAL_TEXTURE2D    = "u_Material.normal";
	const char* ShaderConfig::HEIGHT_TEXTURE2D    = "u_Material.height";
	const char* ShaderConfig::AO_TEXTURE2D        = "u_Material.ao";
	const char* ShaderConfig::ROUGHNESS_TEXTURE2D = "u_Material.roughness";
	const char* ShaderConfig::METALLIC_TEXTURE2D  = "u_Material.mentallic";
	const char* ShaderConfig::DEPTH_TEXTURE2D     = "u_Material.depth";

	const char* ShaderConfig::CUBE_TEXTURECUBE = "u_Material.cube";
	const char* ShaderConfig::SHININESS        = "u_Material.shininess";

	const char* ShaderConfig::IS_BLINNLIGHT              = "u_Settings.isBlinnLight";
	const char* ShaderConfig::IS_AMBIENT_TEXTURE_SAMPLE  = "u_Settings.isAmbientTextureSample";
	const char* ShaderConfig::IS_PBR_TEXTURE_SAMPLE      = "u_Settings.isPBRTextureSample";
	const char* ShaderConfig::IS_DIFFUSE_TEXTURE_SAMPLE  = "u_Settings.isDiffuseTextureSample";
	const char* ShaderConfig::IS_SPECULAR_TEXTURE_SAMPLE = "u_Settings.isSpecularTextureSample";
	const char* ShaderConfig::IS_METALLIC_TEXTURE_SAMPLE = "u_Settings.isMetallicTextureSample";
	const char* ShaderConfig::IS_EMISSION_TEXTURE_SAMPLE = "u_Settings.isEmissionTextureSample";
	const char* ShaderConfig::IS_HEIGHT_TEXTURE_SAMPLE   = "u_isHeightTextureSample";
}


namespace BlackPearl
{
/** Thread ID of the main/game thread																		*/
    uint32_t					GGameThreadId					= 0;
    uint32_t					GRenderThreadId					= 0;
    bool                        GUseThreadedRendering           = false;
    uint32_t	                GAndroidWindowDPI               = 0;
    uint32_t                    GAndroid3DSceneMinDPI           = 0;
    int32_t                     GAndroid3DSceneMaxDesiredPixelCount = 0;

    int32_t GAndroidWindowDPIQueryMethod = 0;





    int MobileUseHWsRGBEncodingCVAR = 0;
    int CVarDisableASTC = 0;
    int CVarOverrideExternalTextureSupport = 1;
    int CVarDisableEarlyFragmentTests = 0;

    int CVarEnableAdrenoTilingHint = 1;
    int CVarDisableFBFNonCoherent = 0;

    int MobileHDRCvar   = 0;
    int CVarStreamingTexturePoolSize = 1000;

    int CVarDisjointTimerQueries = 1;


}