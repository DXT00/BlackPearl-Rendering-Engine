#pragma once
#include"pch.h"
#include "vkShader.h"
#include <BlackPearl/Core.h>
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Core.h"
#include "VkShader.h"
#include "BlackPearl/RHI/RHIPipeline.h"
#include "VkShader.h"
namespace BlackPearl {

	static GLenum ShaderTypeFromString(const std::string& type) {

		/*if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		if (type == "geometry")
			return GL_GEOMETRY_SHADER;
		if (type == "compute")
			return GL_COMPUTE_SHADER;
		if (type == "tessellation_control_shader")
			return GL_TESS_CONTROL_SHADER;
		if (type == "tessellation_evaluation_shader")
			return GL_TESS_EVALUATION_SHADER;
		GE_ASSERT(false, "Unknown shader type!");*/
		return 0;
	}
	vkShader::vkShader(
		const std::string& vertexSrc,
		const std::string& fragmentSrc,
		const std::string& geometrySrc,
		const std::string& tessCtlSrc,
		const std::string& tessEvlSrc
	)
	{
		/*std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
		shaderSources[GL_GEOMETRY_SHADER] = geometrySrc;
		shaderSources[GL_TESS_CONTROL_SHADER] = tessCtlSrc;
		shaderSources[GL_TESS_EVALUATION_SHADER] = tessEvlSrc;

		Compile(shaderSources);*/

	}


	vkShader::vkShader(const std::string& filepath)
	{
		if (filepath.empty()) {

			GE_CORE_WARN("no shader path found");
			return;
		}
		m_ShaderPath = filepath;
		m_Code = _ReadFile(filepath);
		//std::string commonSource = ReadFile(m_CommonStructPath);

		//std::unordered_map<GLenum, std::string> shaderSources = PreProcess(source, commonSource);
		//Compile(shaderSources);

	}

	vkShader::~vkShader()
	{

		

	}


	std::vector<char>  vkShader::_ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end);
			int len = in.tellg();
			std::vector<char> buffer(len);
			in.seekg(0, std::ios::beg);
			in.read(buffer.data(), len);
			in.close();
			return buffer;
		}
		else {
			GE_ASSERT("Could not open file '{0}'", filepath);
		}
		return std::vector<char>();
	}
	std::unordered_map<GLenum, std::string> vkShader::PreProcess(const std::string& source, const std::string& commonSource) {

		GE_ASSERT((!source.empty()), "shader source code is empty");
		std::unordered_map<unsigned int, std::string> shaderSources;
		return shaderSources;

	}
	void vkShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{		
	}

	//void vkShader::SetLightUniform(LightSources lightSources)
	//{
	//	unsigned int pointLightIndex = 0;
	//	this->SetUniform1i("u_PointLightNums", lightSources.GetPointLightNum());
	//	for (auto lightObj : lightSources.Get()) {

	//		if (lightObj->HasComponent<ParallelLight>()) {
	//			auto lightSource = lightObj->GetComponent<ParallelLight>();
	//			this->SetUniform1ui("u_LightType", (unsigned int)LightType::ParallelLight);
	//			this->SetUniform1i("u_HasParallelLight", 1);
	//			this->SetUniformVec3f("u_ParallelLight.ambient", lightSource->GetLightProps().ambient);
	//			this->SetUniformVec3f("u_ParallelLight.diffuse", lightSource->GetLightProps().diffuse);
	//			this->SetUniformVec3f("u_ParallelLight.specular", lightSource->GetLightProps().specular);
	//			this->SetUniformVec3f("u_ParallelLight.direction", lightSource->GetDirection());
	//			this->SetUniform1f("u_ParallelLight.intensity", lightSource->GetLightProps().intensity);

	//		}
	//		if (lightObj->HasComponent<PointLight>()) {

	//			auto lightSource = lightObj->GetComponent<PointLight>();
	//			this->SetUniform1ui("u_LightType", (unsigned int)LightType::PointLight);

	//			this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].ambient", lightSource->GetLightProps().ambient);
	//			this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].diffuse", lightSource->GetLightProps().diffuse);
	//			this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].specular", lightSource->GetLightProps().specular);
	//			this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].position", lightObj->GetComponent<Transform>()->GetPosition());
	//			this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].intensity", lightSource->GetLightProps().intensity);

	//			this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].constant", lightSource->GetAttenuation().constant);
	//			this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].linear", lightSource->GetAttenuation().linear);
	//			this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].quadratic", lightSource->GetAttenuation().quadratic);

	//			pointLightIndex++;
	//		}

	//		if (lightObj->HasComponent<SpotLight>()) {
	//			auto lightSource = lightObj->GetComponent<SpotLight>();

	//			lightSource->UpdatePositionAndDirection(Renderer::GetSceneData()->CameraPosition, Renderer::GetSceneData()->CameraFront);//SpotLight��ʱ��ǵø���Camera

	//			this->SetUniform1ui("u_LightType", (unsigned int)LightType::SpotLight);
	//			this->SetUniform1i("u_HasSpotLight", 1);

	//			this->SetUniformVec3f("u_SpotLight.ambient", lightSource->GetLightProps().ambient);
	//			this->SetUniformVec3f("u_SpotLight.diffuse", lightSource->GetLightProps().diffuse);
	//			this->SetUniformVec3f("u_SpotLight.specular", lightSource->GetLightProps().specular);
	//			this->SetUniformVec3f("u_SpotLight.position", lightSource->GetPosition()); //TODO:Position Ӧ�ô�Transform��
	//			this->SetUniformVec3f("u_SpotLight.direction", lightSource->GetDirection());

	//			this->SetUniform1f("u_SpotLight.cutOff", lightSource->GetCutOffAngle());
	//			this->SetUniform1f("u_SpotLight.outerCutOff", lightSource->GetOuterCutOffAngle());

	//			this->SetUniform1f("u_SpotLight.constant", lightSource->GetAttenuation().constant);
	//			this->SetUniform1f("u_SpotLight.linear", lightSource->GetAttenuation().linear);
	//			this->SetUniform1f("u_SpotLight.quadratic", lightSource->GetAttenuation().quadratic);
	//		}




	//	}

	//}

	void vkShader::Bind() const
	{
		GE_ASSERT((glIsProgram(m_RendererID) == GL_TRUE), "glIsProgram(m_RendererID) != GL_TRUE");

		GLint compileStat;
		//glGetShaderiv(m_RendererID, GL_COMPILE_STATUS, &compileStat);

		GE_ERROR_JUDGE();

		//glGetShaderInfoLog(m_RendererID, 1024, length, infoLog);
		glUseProgram(m_RendererID);

		GLsizei bufLen = 0;        // length of buffer to allocate
		GLsizei strLen = 0;        // strlen GL actually wrote to buffer

		/*glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &bufLen);
		if (bufLen > 1)
		{*/
		GLchar* infoLog = new GLchar[1024];
		glGetProgramInfoLog(m_RendererID, bufLen, &strLen, infoLog);
		if (strLen > 0) {
			std::string  result = reinterpret_cast<char*>(infoLog);
			GE_CORE_INFO(result);

		}
		delete[] infoLog;
		//}


	/*	std::vector<char> v(1024);
		glGetProgramInfoLog(m_RendererID, 1024, NULL, v.data());
		std::string s(begin(v), end(v));
		GE_CORE_INFO(s);*/

		GE_ERROR_JUDGE();
	}

	void vkShader::Unbind() const
	{
		glUseProgram(0);
		GE_ERROR_JUDGE();


	}

	std::vector<char> vkShader::Code()
	{
		return m_Code;
	}

	




	//}
}