#pragma once
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHIShader.h"
#include "Renderer/Shader/GLSLIncluder.h"

namespace BlackPearl {
	//兼容旧的 shader 文件： .glsl里包含.vert 和 .frag
	class MaterialShader
	{
	public:
		MaterialShader(const std::string& filepath, std::vector<std::string>* extentions = nullptr, std::vector<std::string>* macros = nullptr);
		
		std::string GetPath() const {
			return m_ShaderPath;
		}
		ShaderHandle GetVertexShader() const { return m_VertexShader; }
		ShaderHandle GetPixelShader() const { return m_PixelShader; }
		ShaderHandle GetGeometryShader() const { return m_GeometryShader; }
		ShaderHandle GetComputeShader() const { return m_ComputeShader; }

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source, const std::string& commonSource);

		std::string m_ShaderPath;
		std::string m_GlslCode;
		std::string m_CommonStructPath = "assets/shaders/glsl/common/CommonStruct.glsl";
#ifdef GE_PLATFORM_WINDOWS
        std::string m_MacroPath = "assets/shaders/glsl/macro/macro.glsl";
#elif defined(GE_PLATFORM_ANDROID)
        std::string m_MacroPath = "assets/shaders/glsl/macro/macro_android.glsl";

#endif
        //hash code 需要用m_ShaderSources
        std::unordered_map<ShaderType, std::string> m_ShaderSources;
		ShaderHandle m_VertexShader = nullptr;
		ShaderHandle m_PixelShader = nullptr;
		ShaderHandle m_GeometryShader = nullptr;
		ShaderHandle m_ComputeShader = nullptr;



        GLSLIncluder m_GlslIncluder;


	};



}
