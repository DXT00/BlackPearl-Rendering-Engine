#pragma once
#include "D3D12RootSignature.h"
namespace BlackPearl {

	class D3D12LocalRootSignature 
	{
	public:
		D3D12LocalRootSignature(std::initializer_list<RootSignatureElement> elements)
			:m_RootSignatureseElements(elements) {
			m_LocalRootParametersCount = m_RootSignatureseElements.size();
			//m_LocalRootParameters = DBG_NEW CD3DX12_ROOT_PARAMETER[m_LocalRootParametersCount];
		}
		D3D12LocalRootSignature() {
			m_LocalRootParametersCount = 0;
			//m_LocalRootParameters = nullptr;
		}
		~D3D12LocalRootSignature() {
			//GE_SAVE_DELETE(m_LocalRootParameters);
		}
		std::vector<RootSignatureElement> GetRootSignatures() const { return m_RootSignatureseElements; }

		CD3DX12_ROOT_SIGNATURE_DESC GetDesc() const { return m_Desc; }
		int Size() const { return m_LocalRootParametersCount; }

		void SetRootSignatureDesc(CD3DX12_ROOT_PARAMETER *parms, int size, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE) {
			
			GE_ASSERT(size <= MAX_ROOTSIGNATRUE_PARAMETERS, "LocalRootParametersCount cannot exceed MAX_ROOTSIGNATRUE_PARAMETERS");
			GE_ASSERT(size == m_LocalRootParametersCount, "parms size not equal m_LocalRootParametersCount");
			m_LocalRootParameters = parms;
			for (size_t i = 0; i < m_LocalRootParametersCount; i++)
			{
				CD3DX12_ROOT_PARAMETER param;
				RootSignatureElement& rs = m_RootSignatureseElements[i];
				switch (rs.slotType)
				{
				case SlotType::MaterialConstant:
				case SlotType::GeometryIndex:
					m_LocalRootParameters[rs.index].InitAsConstants(rs.constantSize, rs.shaderRegister);
					break;
				default:
					break;
				}
			}
			//CD3DX12_ROOT_SIGNATURE_DESC desc(m_LocalRootParametersCount, m_LocalRootParameters);
			m_Desc = CD3DX12_ROOT_SIGNATURE_DESC(m_LocalRootParametersCount, m_LocalRootParameters);
			m_Desc.Flags = flags;

			//return m_Desc;

		}
	private:
		std::vector<RootSignatureElement> m_RootSignatureseElements;
		//CD3DX12_ROOT_PARAMETER m_LocalRootParameters[MAX_ROOTSIGNATRUE_PARAMETERS];
		CD3DX12_ROOT_PARAMETER* m_LocalRootParameters;
		CD3DX12_ROOT_SIGNATURE_DESC m_Desc;
		int m_LocalRootParametersCount;
	};
}


