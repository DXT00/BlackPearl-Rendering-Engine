#pragma once
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHIShader.h"

namespace BlackPearl {
	//兼容旧的 shader 文件： .glsl里包含.vert 和 .frag
	class MaterialShader
	{
	public:
		MaterialShader(const std::string& filepath);
		
		std::string GetPath() const {
			return m_ShaderPath;
		}
		IShader* GetVertexShader() const { return m_VertexShader; }
		IShader* GetPixelShader() const { return m_PixelShader; }
		IShader* GetGeometryShader() const { return m_GeometryShader; }
		IShader* GetComputeShader() const { return m_ComputeShader; }

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source, const std::string& commonSource);

		std::string m_ShaderPath;
		std::string m_GlslCode;
		std::string m_CommonStructPath = "assets/shaders/common/CommonStruct.glsl";


		IShader* m_VertexShader = nullptr;
		IShader* m_PixelShader = nullptr;
		IShader* m_GeometryShader = nullptr;
		IShader* m_ComputeShader = nullptr;


	};



}
