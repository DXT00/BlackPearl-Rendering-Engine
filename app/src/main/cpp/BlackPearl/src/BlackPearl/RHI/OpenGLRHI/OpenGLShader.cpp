#pragma once
#include"pch.h"
#include <glad/glad.h>
#include "OpenGLShader.h"
#include <BlackPearl/Core.h>
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "../OpenGLRHI/OpenGLDevice.h"
namespace BlackPearl {

	static GLenum ShaderTypeFromString(const std::string&type) {

		if (type == "vertex")
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
		GE_ASSERT(false, "Unknown shader type!");
		return 0;
	}
	Shader::Shader(
		const std::string& vertexSrc, 
		const std::string& fragmentSrc, 
		const std::string& geometrySrc,
		const std::string& tessCtlSrc,
		const std::string& tessEvlSrc
	)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
		shaderSources[GL_GEOMETRY_SHADER] = geometrySrc;
		shaderSources[GL_TESS_CONTROL_SHADER] = tessCtlSrc;
		shaderSources[GL_TESS_EVALUATION_SHADER] = tessEvlSrc;

		Compile(shaderSources);

	}

	Shader::Shader(const std::string & filepath)
	{
		if (filepath.empty()) {

			GE_CORE_WARN("no shader path found");
			return ;
		}
		m_ShaderPath = filepath;
		std::string source = ReadFile(filepath);
		std::string commonSource = ReadFile(m_CommonStructPath);

		std::unordered_map<GLenum, std::string> shaderSources = PreProcess(source, commonSource);
		Compile(shaderSources);

	}

	Shader::~Shader()
	{

		if (m_RendererID == -1) {
			return;
		}
		GLint has_deleted = 0;
		glGetProgramiv(m_RendererID, GL_DELETE_STATUS, &has_deleted);
		if (has_deleted != GL_TRUE)
		{
			glDeleteProgram(m_RendererID);
		}
		
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
	std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source,const std::string& commonSource) {

		GE_ASSERT((!source.empty()), "shader source code is empty");
		std::unordered_map<unsigned int, std::string> shaderSources;
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
		//add common struct source
		if (shaderSources.find(GL_FRAGMENT_SHADER)!=shaderSources.end()) {
			size_t pos = shaderSources[GL_FRAGMENT_SHADER].find("#version", 0);//find找不到会返回npos
			GE_ASSERT(pos != std::string::npos, "Syntax error");

			size_t eol = shaderSources[GL_FRAGMENT_SHADER].find_first_of("\r\n", pos);
			GE_ASSERT(eol != std::string::npos, "Syntax error");

			std::string front = shaderSources[GL_FRAGMENT_SHADER].substr(pos, eol - pos + 1);
			std::string res = shaderSources[GL_FRAGMENT_SHADER].substr(eol);
			shaderSources[GL_FRAGMENT_SHADER] = front + commonSource + res;
		}
		return shaderSources;

	}
	void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		GE_ERROR_JUDGE();

		std::vector<GLuint> ShaderID;
		for (auto& kv : shaderSources) {
			GLenum type = kv.first;
			const std::string& source = kv.second;
			GLuint shader = glCreateShader(type);
			GE_ERROR_JUDGE();
			ShaderID.push_back(shader);

			const GLchar *sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);
			GE_ERROR_JUDGE();
			glCompileShader(shader);
			GE_ERROR_JUDGE();
			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			GE_ERROR_JUDGE();
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
				GE_ERROR_JUDGE();

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
				GE_ERROR_JUDGE();
				glDeleteShader(shader);
				GE_ERROR_JUDGE();

				std::string shaderType;
				if (type == GL_VERTEX_SHADER)shaderType = "vertex shader";
				else if (type == GL_FRAGMENT_SHADER)shaderType = "fragment shader";
				else if (type == GL_GEOMETRY_SHADER)shaderType = "geometry shader";
				else if (type == GL_COMPUTE_SHADER)shaderType = "compute shader";
				else if (type == GL_TESS_CONTROL_SHADER)shaderType = "tessellation control shader";
				else if (type == GL_TESS_EVALUATION_SHADER)shaderType = "tessellation evaluation shader";

				
				GE_CORE_ERROR("{0} compile failed :{1}", shaderType,infoLog.data());
				GE_ASSERT(false, "Shader compliation failure!")

					break;
			}
			// Attach our shaders to our program
			glAttachShader(program, shader);
			GE_ERROR_JUDGE();
		}


		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.

		// Link our program
		glLinkProgram(program);
		GE_ERROR_JUDGE();
		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		GE_ERROR_JUDGE();
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			GE_ERROR_JUDGE();
			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			GE_ERROR_JUDGE();
			// We don't need the program anymore.
			glDeleteProgram(program);
			GE_ERROR_JUDGE();
			// Don't leak shaders either.
			for (auto& shader : ShaderID) {
				glDeleteShader(shader);
				GE_ERROR_JUDGE();
			}

			GE_CORE_ERROR("{0}", infoLog.data());
			GE_ASSERT(false, "Shader link failure!")

				return;
		}

		glValidateProgram(program);
		GE_ERROR_JUDGE();
		
		// Always detach shaders after a successful link.
		for (auto& shader : ShaderID) {
			glDetachShader(program, shader);
			GE_ERROR_JUDGE();
		}
		m_RendererID = program;
	}

	void Shader::SetLightUniform( LightSources lightSources)
	{
		unsigned int pointLightIndex = 0;
		this->SetUniform1i("u_PointLightNums", lightSources.GetPointLightNum());
		for (auto lightObj : lightSources.Get()) {

			if (lightObj->HasComponent<ParallelLight>()) {
				auto lightSource = lightObj->GetComponent<ParallelLight>();
				this->SetUniform1ui("u_LightType", (unsigned int)LightType::ParallelLight);
				this->SetUniform1i("u_HasParallelLight", 1);
				this->SetUniformVec3f("u_ParallelLight.ambient", lightSource->GetLightProps().ambient);
				this->SetUniformVec3f("u_ParallelLight.diffuse", lightSource->GetLightProps().diffuse);
				this->SetUniformVec3f("u_ParallelLight.specular", lightSource->GetLightProps().specular);
				this->SetUniformVec3f("u_ParallelLight.direction", lightSource->GetDirection());
				this->SetUniform1f("u_ParallelLight.intensity", lightSource->GetLightProps().intensity);

			}
			if (lightObj->HasComponent<PointLight>()) {

				auto lightSource = lightObj->GetComponent<PointLight>();
				this->SetUniform1ui("u_LightType", (unsigned int)LightType::PointLight);

				this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].ambient", lightSource->GetLightProps().ambient);
				this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].diffuse", lightSource->GetLightProps().diffuse);
				this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].specular", lightSource->GetLightProps().specular);
				this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].position", lightObj->GetComponent<Transform>()->GetPosition());
				this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].intensity", lightSource->GetLightProps().intensity);

				this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].constant", lightSource->GetAttenuation().constant);
				this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].linear",lightSource->GetAttenuation().linear);
				this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].quadratic", lightSource->GetAttenuation().quadratic);

				pointLightIndex++;
			}
			
			if (lightObj->HasComponent<SpotLight>()) {
				auto lightSource = lightObj->GetComponent<SpotLight>();

				lightSource->UpdatePositionAndDirection(Renderer::GetSceneData()->CameraPosition, Renderer::GetSceneData()->CameraFront);//SpotLight的时候记得更新Camera

				this->SetUniform1ui("u_LightType", (unsigned int)LightType::SpotLight);
				this->SetUniform1i("u_HasSpotLight", 1);

				this->SetUniformVec3f("u_SpotLight.ambient", lightSource->GetLightProps().ambient);
				this->SetUniformVec3f("u_SpotLight.diffuse", lightSource->GetLightProps().diffuse);
				this->SetUniformVec3f("u_SpotLight.specular", lightSource->GetLightProps().specular);
				this->SetUniformVec3f("u_SpotLight.position",lightSource->GetPosition()); //TODO:Position 应该从Transform拿
				this->SetUniformVec3f("u_SpotLight.direction", lightSource->GetDirection());

				this->SetUniform1f("u_SpotLight.cutOff",lightSource->GetCutOffAngle());
				this->SetUniform1f("u_SpotLight.outerCutOff", lightSource->GetOuterCutOffAngle());

				this->SetUniform1f("u_SpotLight.constant", lightSource->GetAttenuation().constant);
				this->SetUniform1f("u_SpotLight.linear", lightSource->GetAttenuation().linear);
				this->SetUniform1f("u_SpotLight.quadratic",lightSource->GetAttenuation().quadratic);
			}

			
			
			
		}

	}

	void Shader::Bind() const
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

	void Shader::Unbind() const
	{
		glUseProgram(0);
		GE_ERROR_JUDGE();


	}

	void Shader::SetUniform1i(const std::string & name, int val) const
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		GE_ERROR_JUDGE();

		if(loc!=-1)
			glUniform1i(loc, val);
		GE_ERROR_JUDGE();

	}

	void Shader::SetUniform1ui(const std::string & name, const unsigned int val) const
	{
		glUniform1ui(glGetUniformLocation(m_RendererID, name.c_str()), val);
		GE_ERROR_JUDGE();

	}
	void Shader::SetUniform1f(const std::string & name, float val) const
	{
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), val);
		GE_ERROR_JUDGE();

	}

	void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &mat) const
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
		GE_ERROR_JUDGE();

	}

	void Shader::SetUniformMat3x4f(const std::string& name, const float* mat3x4, uint32_t count) const
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glProgramUniformMatrix3x4fv(m_RendererID, location, count, GL_FALSE, mat3x4);
		GE_ERROR_JUDGE();

	}

	void Shader::SetUniformMat4f(const std::string& name, const float* mat4x4, uint32_t count) const
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glProgramUniformMatrix4fv(m_RendererID, location, count, GL_FALSE, mat4x4);
	}

	void Shader::SetUniformVec3f(const std::string & name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
		GE_ERROR_JUDGE();

	}
	void Shader::SetUniformVec2f(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
		GE_ERROR_JUDGE();

	}

	void Shader::SetUniformVec3f(const std::string& name, const math::float3& value) const
	{
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
		GE_ERROR_JUDGE();

	}
	void Shader::SetUniformVec2f(const std::string& name, const math::float2& value) const
	{
		glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
		GE_ERROR_JUDGE();

	}
	void Shader::SetUniformVec2i(const std::string& name, const glm::ivec2& value) const
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		GE_ERROR_JUDGE();

		glUniform2iv(location, 1, &value[0]);
		GE_ERROR_JUDGE();

	}

	void Device::BindUniformBufferBase(FOpenGLContextState& ContextState, int32_t NumUniformBuffers, uint32_t** BoundUniformBuffers, uint32_t FirstUniformBuffer, bool ForceUpdate)
	{
		/*SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLUniformBindTime);
		VERIFY_GL_SCOPE();
		checkSlow(IsInRenderingThread() || IsInRHIThread());*/

		for (int32_t BufferIndex = 0; BufferIndex < NumUniformBuffers; ++BufferIndex)
		{
			GLuint Buffer = 0;
			uint32_t Offset = 0;
			uint32_t Size = ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE;
			int32_t BindIndex = FirstUniformBuffer + BufferIndex;

			if (BoundUniformBuffers[BufferIndex])
			{
	/*			FRHIUniformBuffer* UB = BoundUniformBuffers[BufferIndex];
				FOpenGLUniformBuffer* GLUB = ((FOpenGLUniformBuffer*)UB);*/
				Buffer = (GLuint)BoundUniformBuffers[BufferIndex]; //GLUB->Resource;

				if (GLUB->bIsEmulatedUniformBuffer)
				{
					continue;
				}

				Size = GLUB->GetSize();
#if SUBALLOCATED_CONSTANT_BUFFER
				Offset = GLUB->Offset;
#endif
			}
			else
			{
				if (PendingState.ZeroFilledDummyUniformBuffer == 0)
				{
					void* ZeroBuffer = malloc(ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE);
					FMemory::Memzero(ZeroBuffer, ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE);
					FOpenGL::GenBuffers(1, &PendingState.ZeroFilledDummyUniformBuffer);
					assert(PendingState.ZeroFilledDummyUniformBuffer != 0);
					CachedBindUniformBuffer(ContextState, PendingState.ZeroFilledDummyUniformBuffer);
					glBufferData(GL_UNIFORM_BUFFER, ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE, ZeroBuffer, GL_STATIC_DRAW);
					delete ZeroBuffer;

					OpenGLBufferStats::UpdateUniformBufferStats(ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE, true);
				}

				Buffer = PendingState.ZeroFilledDummyUniformBuffer;
			}

			if (ForceUpdate || (Buffer != 0 && ContextState.UniformBuffers[BindIndex] != Buffer) || ContextState.UniformBufferOffsets[BindIndex] != Offset)
			{
				FOpenGL::BindBufferRange(GL_UNIFORM_BUFFER, BindIndex, Buffer, Offset, Size);
				ContextState.UniformBuffers[BindIndex] = Buffer;
				ContextState.UniformBufferOffsets[BindIndex] = Offset;
				ContextState.UniformBufferBound = Buffer;	// yes, calling glBindBufferRange also changes uniform buffer binding.
			}
		}
	}
	void Device::BindPendingShaderState(FOpenGLContextState& ContextState)
	{
		bool ForceUniformBindingUpdate = false;

		GLuint PendingProgram = PendingState.GraphicsPipline->shaderLinkProgram;
		if (ContextState.Program != PendingProgram)
		{
			FOpenGL::BindProgramPipeline(PendingProgram);
			ContextState.Program = PendingProgram;
			//MarkShaderParameterCachesDirty(PendingState.ShaderParameters, false);
			//PendingState.LinkedProgramAndDirtyFlag = nullptr;
		}

		if (PendingState.bAnyDirtyRealUniformBuffers[(int)ShaderType::Vertex] ||
			PendingState.bAnyDirtyRealUniformBuffers[(int)ShaderType::Pixel] ||
			PendingState.bAnyDirtyRealUniformBuffers[(int)ShaderType::Geometry])
		{
			int32_t NextUniformBufferIndex = OGL_FIRST_UNIFORM_BUFFER;

			/*static_assert(SF_NumGraphicsFrequencies == 5 && SF_NumFrequencies == 10, "Unexpected SF_ ordering");
			static_assert(SF_RayGen > SF_NumGraphicsFrequencies, "SF_NumGraphicsFrequencies be the number of frequencies supported in OpenGL");*/

			int32_t NumUniformBuffers[(int)ShaderType::AllGraphics];

			PendingState.GraphicsPipline->pipelineBindingLayouts->getNumUniformBuffers(NumUniformBuffers);

			if (PendingState.bAnyDirtyRealUniformBuffers[(int)ShaderType::Vertex])
			{
				BindUniformBufferBase(
					ContextState,
					NumUniformBuffers[(int)ShaderType::Vertex],
					PendingState.BoundUniformBuffers[(int)ShaderType::Vertex],
					NextUniformBufferIndex,
					ForceUniformBindingUpdate);
			}
			NextUniformBufferIndex += NumUniformBuffers[(int)ShaderType::Vertex];

			if (PendingState.bAnyDirtyRealUniformBuffers[(int)ShaderType::Pixel])
			{
				BindUniformBufferBase(
					ContextState,
					NumUniformBuffers[(int)ShaderType::Pixel]],
					PendingState.BoundUniformBuffers[SF_Pixel],
					NextUniformBufferIndex,
					ForceUniformBindingUpdate);
			}
			NextUniformBufferIndex += NumUniformBuffers[(int)ShaderType::Pixel];

			if (NumUniformBuffers[(int)ShaderType::Geometry] >= 0 && PendingState.bAnyDirtyRealUniformBuffers[SF_Geometry])
			{
				BindUniformBufferBase(
					ContextState,
					NumUniformBuffers[SF_Geometry],
					PendingState.BoundUniformBuffers[SF_Geometry],
					NextUniformBufferIndex,
					ForceUniformBindingUpdate);
				NextUniformBufferIndex += NumUniformBuffers[SF_Geometry];
			}

			PendingState.bAnyDirtyRealUniformBuffers[(int)ShaderType::Vertex] = false;
			PendingState.bAnyDirtyRealUniformBuffers[(int)ShaderType::Pixel] = false;
			PendingState.bAnyDirtyRealUniformBuffers[(int)ShaderType::Geometry] = false;
		}

		if (FOpenGL::SupportsBindlessTexture())
		{
			SetupBindlessTextures(ContextState, PendingState.BoundShaderState->LinkedProgram->Samplers);
		}
	}
}