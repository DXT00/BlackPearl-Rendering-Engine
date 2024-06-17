#pragma once
#ifdef GE_API_D3D12
#include <d3d12.h>
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/Renderer/Buffer/Buffer.h"
namespace BlackPearl {

	/*DXGI_FORMAT GetD3D12InputElementFormat(ElementDataType type) {
		switch (type) {
		case ElementDataType::Int:      return DXGI_FORMAT_R32_UINT;
		case ElementDataType::Int2:     return DXGI_FORMAT_R32G32_UINT;
		case ElementDataType::Int3:     return DXGI_FORMAT_R32G32B32_UINT;
		case ElementDataType::Int4:     return DXGI_FORMAT_R32G32B32A32_UINT;
		case ElementDataType::Float:    return DXGI_FORMAT_R32_FLOAT;
		case ElementDataType::Float2:   return DXGI_FORMAT_R32G32_FLOAT;
		case ElementDataType::Float3:   return DXGI_FORMAT_R32G32B32_FLOAT;
		case ElementDataType::Float4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;

		}
		GE_ASSERT(false, "Unknown ElementDataType!")
		return DXGI_FORMAT_UNKNOWN;
	}

	class D3D12VertexBufferLayout : public VertexBufferLayout
	{
	public:
		D3D12VertexBufferLayout()
			:VertexBufferLayout(){}

		D3D12VertexBufferLayout(std::initializer_list<BufferElement> elements)
		:VertexBufferLayout(elements){}

		const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetDesc() const {
			return m_Descs;
		}
		virtual void UpdateDesc() override{
			m_Descs.clear();
			for (auto& element : m_Elememts)
			{
				D3D12_INPUT_ELEMENT_DESC desc = {};
				desc.SemanticName = element.Name.c_str();
				desc.SemanticIndex = 0;
				desc.Format = GetD3D12InputElementFormat(element.Type);
				GE_ASSERT(element.Location < 16, "DirextX support only 0-15 input slot");
				desc.InputSlot = element.Location;
				desc.AlignedByteOffset = element.Offset;
				desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				desc.InstanceDataStepRate = 1;
				m_Descs.push_back(desc);
			}
		}

	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_Descs;
	};



	class D3D12VertexBuffer
	{
	public:
	};*/

}
#endif