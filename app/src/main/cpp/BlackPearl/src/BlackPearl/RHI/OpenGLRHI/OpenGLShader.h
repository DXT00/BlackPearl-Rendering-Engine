#pragma once
#include "glad/glad.h"
#include<glm/glm.hpp>
#include<string>
#include<memory>
#include <unordered_map>
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Math/vector.h"
#include "BlackPearl/RHI/RHIShader.h"
#include <BlackPearl/Renderer/Renderer.h>

namespace BlackPearl {


	class Shader : public RefCounter<IShader>
	{
	public:
		ShaderDesc desc;
		//TODO:: use glProgramBinary
		Shader(const ShaderDesc& _desc, const void* binaryCode, size_t binarySize);
		Shader(const ShaderDesc& _desc, const std::string& filepath);
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
		void SetUniformMat4f(const std::string& name, const float* mat4x4, uint32_t count) const;

		void SetUniformMat3x4f(const std::string& name, const float* mat3x4, uint32_t count = 1) const;

		void SetUniformVec3f(const std::string & name, const glm::vec3& value) const;
		void SetUniformVec2f(const std::string& name, const glm::vec2& value) const;
		void SetUniformVec3f(const std::string& name, const math::float3& value) const;
		void SetUniformVec2f(const std::string& name, const math::float2& value) const;
		void SetUniformVec2i(const std::string& name, const glm::ivec2& value) const;
		

		std::string GetPath() { return m_ShaderPath; }
		const ShaderDesc& getDesc() const override { return desc; }
		void getBytecode(const void** ppBytecode, size_t* pSize) const override;
	private:
		Shader(
			const std::string& vertexSrc, 
			const std::string& fragmentSrc, 
			const std::string& geometrySrc, 
			const std::string& tessCtlSrc,
			const std::string& tessEvlSrc);

		int32_t m_RendererID = -1;
		std::string m_ShaderPath;
		std::string m_FragmentCommonStruct;
		std::string m_CommonStructPath="assets/shaders/common/CommonStruct.glsl";


	};

    class FOpenGLLinkedProgram {
    public :
        GLuint		Program;
        bool		bDrawn;
    };
}