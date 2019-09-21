#pragma once
#include"pch.h"
#include "Shader.h"
#include<glad/glad.h>
#include<GameEngine/Core.h>
#include"Lighting/ParallelLight.h"
#include"Lighting/PointLight.h"
#include"Lighting/SpotLight.h"
#include "Lighting/Light.h"
#include "Renderer.h"
#include "Lighting/LightSources.h"
static GLenum ShaderTypeFromString(const std::string&type) {

	if (type == "vertex")
		return GL_VERTEX_SHADER;
	if (type == "fragment" || type == "pixel")
		return GL_FRAGMENT_SHADER;
	GE_ASSERT(false, "Unknown shader type!");
	return 0;
}
Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
{
	std::unordered_map<GLenum, std::string> shaderSources;
	shaderSources[GL_VERTEX_SHADER] = vertexSrc;
	shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
	Compile(shaderSources);

}

Shader::Shader(const std::string & filepath)
{
	std::string source = ReadFile(filepath);
	std::unordered_map<GLenum, std::string> shaderSources = PreProcess(source);
	Compile(shaderSources);

}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}


std::string Shader::ReadFile(const std::string & filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in) {
		in.seekg(0, std::ios::end);
		int len = in.tellg();
		result.resize(len);
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else {
		GE_ASSERT("Could not open file '{0}'", filepath);
	}
	return result;
}
std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source) {

	std::unordered_map<GLenum, std::string> shaderSources;
	const char* typeToken = "#type";
	size_t typeTockenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0);//find找不到会返回npos
	while (pos != std::string::npos) {
		size_t eol = source.find_first_of("\r\n", pos);
		GE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + typeTockenLength + 1;
		std::string type = source.substr(begin, eol - begin);
		GE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

		size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		pos = source.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos,
			pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));//string::npos表示source的末尾位置

	}
	return shaderSources;

}
void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
{
	GLuint program = glCreateProgram();


	std::vector<GLuint> ShaderID;
	for (auto& kv : shaderSources) {
		GLenum type = kv.first;
		const std::string& source = kv.second;
		GLuint shader = glCreateShader(type);
		ShaderID.push_back(shader);

		const GLchar *sourceCstr = source.c_str();
		glShaderSource(shader, 1, &sourceCstr, 0);

		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			GE_CORE_ERROR("{0}", infoLog.data());
			GE_ASSERT(false, "Shader compliation failure!")

			break;
		}
		// Attach our shaders to our program
		glAttachShader(program, shader);

	}


	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		for (auto& shader : ShaderID) {
			glDeleteShader(shader);
		}

		GE_CORE_ERROR("{0}", infoLog.data());
		GE_ASSERT(false, "Shader link failure!")

		return;
	}
	// Always detach shaders after a successful link.
	for (auto& shader : ShaderID) {
		glDetachShader(program, shader);
	}
	m_RendererID = program;
}

void Shader::SetLightUniform(const LightSources& lightSources)
{
	unsigned int pointLightIndex = 0;
	this->SetUniform1i("u_PointLightNums", lightSources.GetPointLightNum());
	for (auto lightSource : lightSources.Get()) {

	switch (lightSource->GetType())
	{
		case LightType::ParallelLight:
			this->SetUniform1ui("u_LightType", (unsigned int)LightType::ParallelLight);

			this->SetUniformVec3f("u_ParallelLight.ambient",   lightSource->GetLightProps().ambient);
			this->SetUniformVec3f("u_ParallelLight.diffuse",   lightSource->GetLightProps().diffuse);
			this->SetUniformVec3f("u_ParallelLight.specular",  lightSource->GetLightProps().specular);
			this->SetUniformVec3f("u_ParallelLight.direction", std::dynamic_pointer_cast<ParallelLight>(lightSource)->GetDirection());
			break;
		case LightType::PointLight:
			this->SetUniform1ui("u_LightType", (unsigned int)LightType::PointLight);

			this->SetUniformVec3f("u_PointLights["+std::to_string(pointLightIndex)+"].ambient",   lightSource->GetLightProps().ambient);
			this->SetUniformVec3f("u_PointLights["+std::to_string(pointLightIndex)+"].diffuse",   lightSource->GetLightProps().diffuse);
			this->SetUniformVec3f("u_PointLights["+std::to_string(pointLightIndex)+"].specular",  lightSource->GetLightProps().specular);
			this->SetUniformVec3f("u_PointLights["+std::to_string(pointLightIndex)+"].position",  std::dynamic_pointer_cast<PointLight>(lightSource)->GetPosition());
		
			this->SetUniform1f("u_PointLights["+std::to_string(pointLightIndex)+"].constant",     std::dynamic_pointer_cast<PointLight>(lightSource)->GetAttenuation().constant);
			this->SetUniform1f("u_PointLights["+std::to_string(pointLightIndex)+"].linear",       std::dynamic_pointer_cast<PointLight>(lightSource)->GetAttenuation().linear);
			this->SetUniform1f("u_PointLights["+std::to_string(pointLightIndex)+"].quadratic",    std::dynamic_pointer_cast<PointLight>(lightSource)->GetAttenuation().quadratic);
			pointLightIndex++;
			break;
		case LightType::SpotLight:
			std::dynamic_pointer_cast<SpotLight>(lightSource)->UpdatePositionAndDirection(Renderer::GetSceneData()->CameraPosition,Renderer::GetSceneData()->CameraFront);//SpotLight的时候记得更新Camera

			this->SetUniform1ui("u_LightType", (unsigned int)LightType::SpotLight);

			this->SetUniformVec3f("u_SpotLight.ambient",   lightSource->GetLightProps().ambient);
			this->SetUniformVec3f("u_SpotLight.diffuse",   lightSource->GetLightProps().diffuse);
			this->SetUniformVec3f("u_SpotLight.specular",  lightSource->GetLightProps().specular);
			this->SetUniformVec3f("u_SpotLight.position",  std::dynamic_pointer_cast<SpotLight>(lightSource)->GetPosition());
			this->SetUniformVec3f("u_SpotLight.direction", std::dynamic_pointer_cast<SpotLight>(lightSource)->GetDirection());

			this->SetUniform1f("u_SpotLight.cutOff",       std::dynamic_pointer_cast<SpotLight>(lightSource)->GetCutOffAngle());
			this->SetUniform1f("u_SpotLight.outerCutOff",  std::dynamic_pointer_cast<SpotLight>(lightSource)->GetOuterCutOffAngle());

			this->SetUniform1f("u_SpotLight.constant",     std::dynamic_pointer_cast<SpotLight>(lightSource)->GetAttenuation().constant);
			this->SetUniform1f("u_SpotLight.linear",       std::dynamic_pointer_cast<SpotLight>(lightSource)->GetAttenuation().linear);
			this->SetUniform1f("u_SpotLight.quadratic",    std::dynamic_pointer_cast<SpotLight>(lightSource)->GetAttenuation().quadratic);
			break;

		default:
			GE_CORE_ERROR("SetLightUniform failed! Unknown Light Type!")
			break;
		}
	}

}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);

}

void Shader::SetUniform1i(const std::string & name, int val) const
{
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), val);
}

void Shader::SetUniform1ui(const std::string & name,const unsigned int val) const
{
	glUniform1ui(glGetUniformLocation(m_RendererID, name.c_str()), val);
}
void Shader::SetUniform1f(const std::string & name, float val) const
{
	glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), val);
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &mat) const
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniformVec3f(const std::string & name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
}

