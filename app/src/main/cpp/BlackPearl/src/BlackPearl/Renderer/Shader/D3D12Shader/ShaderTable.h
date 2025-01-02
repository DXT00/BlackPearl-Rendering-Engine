#pragma once
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/GpuUploadBuffer.h"
#include "ShaderRecord.h"
namespace BlackPearl {
	class ShaderTable :public GpuUploadBuffer
	{
	public:
		ShaderTable(ID3D12Device* device, UINT numShaderRecord, UINT shaderRecordSize, LPCWSTR resourceName = nullptr)
			:m_Name(resourceName)
		{
			m_ShaderRecordSize = Align(shaderRecordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
			m_ShaderRecords.reserve(numShaderRecord);
			UINT bufferSize = numShaderRecord * m_ShaderRecordSize;
			Allocate(device, bufferSize, resourceName);
			m_MappedShaderRecords = MapCpuWriteOnly();

		}
		
		//TODO
		~ShaderTable() = default;
		UINT GetShaderRecordSize() const { return m_ShaderRecordSize; }
		void push_back(const ShaderRecord& shaderRecord) {
			ThrowIfFalse(m_ShaderRecords.size() < m_ShaderRecords.capacity());
			m_ShaderRecords.push_back(shaderRecord);
			shaderRecord.CopyTo(m_MappedShaderRecords);
			m_MappedShaderRecords += m_ShaderRecordSize;
		}

		// Pretty-print the shader records.
		void DebugPrint(std::unordered_map<void*, std::wstring> shaderIdToStringMap)
		{
			std::wstringstream wstr;
			wstr << L"|--------------------------------------------------------------------\n";
			wstr << L"|Shader table - " << m_Name.c_str() << L": "
				<< m_ShaderRecordSize << L" | "
				<< m_ShaderRecords.size() * m_ShaderRecordSize << L" bytes\n";

			for (UINT i = 0; i < m_ShaderRecords.size(); i++)
			{
				wstr << L"| [" << i << L"]: ";
				wstr << shaderIdToStringMap[m_ShaderRecords[i].m_ShaderIdentifier.ptr] << L", ";
				wstr << m_ShaderRecords[i].m_ShaderIdentifier.size << L" + " << m_ShaderRecords[i].m_RootArgument.size << L" bytes \n";
			}
			wstr << L"|--------------------------------------------------------------------\n";
			wstr << L"\n";
			OutputDebugStringW(wstr.str().c_str());
		}
	private:
		ShaderTable() {}
		uint8_t* m_MappedShaderRecords;
		std::vector<ShaderRecord> m_ShaderRecords;
		UINT m_ShaderRecordSize;
		std::wstring m_Name;
	};

}

