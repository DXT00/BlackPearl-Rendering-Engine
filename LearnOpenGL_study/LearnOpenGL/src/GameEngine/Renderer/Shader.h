#pragma once
#include<glm/glm.hpp>
#include<string>
#include<memory>
#include<glad/glad.h>
#include"Lighting/LightType.h"
#include"Lighting/Light.h"

class Shader
{
public:
	Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
	Shader(const std::string& filepath);
	~Shader();
	void Bind()const;
	void Unbind() const;
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source);
	void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	void SetLightUniform(LightType::Type type, const std::shared_ptr<Light>& m_LightSource);

	void SetUniform1i(const std::string &name,int val) const;
	void SetUniform1f(const std::string &name, float val) const;
	void SetUniformMat4f(const std::string &name, const glm::mat4 &mat) const;
	void SetUniformVec3f(const std::string & name, const glm::vec3& value) const;


private:
	uint32_t m_RendererID;


};
