#pragma once
#include "glad/glad.h"
#include<glm/glm.hpp>
#include<string>
#include<memory>
#include <unordered_map>
#include "BlackPearl/Component/LightComponent/LightSources.h"
namespace BlackPearl {


	class vkShader
	{
	public:

		vkShader(const std::string& filepath);
		~vkShader();
		void Bind()const;
		void Unbind() const;
		std::vector<char> Code();
		//std::string ReadFile(const std::string& filepath);

		std::unordered_map<GLenum, std::string> vkShader::PreProcess(const std::string& source, const std::string& commonSource);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		//void SetLightUniform(LightSources lightSources);

		/*void SetUniform1i(const std::string& name, int val) const;
		void SetUniform1ui(const std::string& name, const unsigned int val) const;
		void SetUniform1f(const std::string& name, float val) const;
		void SetUniformMat4f(const std::string& name, const glm::mat4& mat) const;
		void SetUniformMat4f(const std::string& name, const float* mat4x4, uint32_t count) const;

		void SetUniformMat3x4f(const std::string& name, const float* mat3x4, uint32_t count = 1) const;

		void SetUniformVec3f(const std::string& name, const glm::vec3& value) const;
		void SetUniformVec2f(const std::string& name, const glm::vec2& value) const;
		void SetUniformVec2i(const std::string& name, const glm::ivec2& value) const;*/


		std::string GetPath() { return m_ShaderPath; }

	private:
		vkShader(
			const std::string& vertexSrc,
			const std::string& fragmentSrc,
			const std::string& geometrySrc,
			const std::string& tessCtlSrc,
			const std::string& tessEvlSrc);
		std::vector<char> _ReadFile(const std::string& filepath);

		int32_t m_RendererID = -1;
		std::string m_ShaderPath;
		std::string m_FragmentCommonStruct;
		std::string m_CommonStructPath = "assets/shaders/common/CommonStruct.glsl";
		std::vector<char> m_Code;


	};
}