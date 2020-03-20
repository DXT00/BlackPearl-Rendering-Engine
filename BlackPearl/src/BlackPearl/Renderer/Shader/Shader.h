#pragma once
#include<glm/glm.hpp>
#include<string>
#include<memory>
#include "glad/glad.h"
#include <unordered_map>
#include "BlackPearl/Component/LightComponent/LightSources.h"
namespace BlackPearl {


	class Shader
	{
	public:
		
		Shader(const std::string& filepath);
		~Shader();
		void Bind()const;
		void Unbind() const;
		std::string ReadFile(const std::string& filepath);
		
		std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source, const std::string& commonSource);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		void SetLightUniform( LightSources lightSources);

		void SetUniform1i(const std::string &name, int val) const;
		void SetUniform1ui(const std::string & name, const unsigned int val) const;
		void SetUniform1f(const std::string &name, float val) const;
		void SetUniformMat4f(const std::string &name, const glm::mat4 &mat) const;
		void SetUniformVec3f(const std::string & name, const glm::vec3& value) const;
		void SetUniformVec2f(const std::string& name, const glm::vec2& value) const;

		virtual void SetExtraUniform() {};

		std::string GetPath() { return m_ShaderPath; }

	private:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc);
		uint32_t m_RendererID;
		std::string m_ShaderPath;
		std::string m_FragmentCommonStruct;
		std::string m_CommonStructPath="assets/shaders/common/CommonStruct.glsl";


	};
}