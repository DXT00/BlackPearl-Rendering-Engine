#include "pch.h"
#include "Config.h"
namespace BlackPearl {


	//const VoxelConeTracingRenderer::RenderingMode Configuration::RenderingMode = VoxelConeTracingRenderer::RenderingMode::VOXEL_CONE_TRACING;
	const float Configuration::ShadowMapPointLightRadius = 30.0f;

	/* 这里改了，prefilterMap.glsl里的	float resolution =512.0;也要改 */
	const float Configuration::EnvironmantMapResolution = 32.0f;
	//const float Configuration::GICoeefs = 0.5f;


	const char* ShaderConfig::AMBIENT_COLOR = "u_Material.ambientColor";
	const char* ShaderConfig::DIFFUSE_COLOR = "u_Material.diffuseColor";
	const char* ShaderConfig::SPECULAR_COLOR = "u_Material.specularColor";
	const char* ShaderConfig::EMISSION_COLOR = "u_Material.emissionColor";
	const char* ShaderConfig::METALLIC_VALUE = "u_Material.mentallicValue";

	const char* ShaderConfig::DIFFUSE_TEXTURE2D = "u_Material.diffuse";
	const char* ShaderConfig::SPECULAR_TEXTURE2D = "u_Material.specular";
	const char* ShaderConfig::EMISSION_TEXTURE2D = "u_Material.emission";
	const char* ShaderConfig::NORMAL_TEXTURE2D = "u_Material.normal";
	const char* ShaderConfig::HEIGHT_TEXTURE2D = "u_Material.height";
	const char* ShaderConfig::AO_TEXTURE2D = "u_Material.ao";
	const char* ShaderConfig::ROUGHNESS_TEXTURE2D = "u_Material.roughness";
	const char* ShaderConfig::METALLIC_TEXTURE2D = "u_Material.mentallic";
	const char* ShaderConfig::DEPTH_TEXTURE2D = "u_Material.depth";

	const char* ShaderConfig::CUBE_TEXTURECUBE = "u_Material.cube";
	const char* ShaderConfig::SHININESS = "u_Material.shininess";

	const char* ShaderConfig::IS_BLINNLIGHT = "u_Settings.isBlinnLight";
	const char* ShaderConfig::IS_TEXTURE_SAMPLE = "u_Settings.isTextureSample";
	const char* ShaderConfig::IS_DIFFUSE_TEXTURE_SAMPLE = "u_Settings.isDiffuseTextureSample";
	const char* ShaderConfig::IS_SPECULAR_TEXTURE_SAMPLE = "u_Settings.isSpecularTextureSample";
	const char* ShaderConfig::IS_METALLIC_TEXTURE_SAMPLE = "u_Settings.isMetallicTextureSample";
}