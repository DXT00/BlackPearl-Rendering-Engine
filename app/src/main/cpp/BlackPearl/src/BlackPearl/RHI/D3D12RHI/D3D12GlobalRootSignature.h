#include "pch.h"
#ifdef GE_API_D3D12
#include "D3D12RootSignature.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {
	class D3D12GlobalRootSignature
	{
	public:
		D3D12GlobalRootSignature(std::initializer_list<RootSignatureElement> elements)
			:m_RootSignatureseElements(elements) {

		}
		~D3D12GlobalRootSignature() = default;
		D3D12_ROOT_SIGNATURE_DESC SetRootSignatureDesc(D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE) {

			GE_ASSERT(m_RootSignatureseElements.size() <= MAX_ROOTSIGNATRUE_PARAMETERS, "LocalRootParametersCount cannot exceed MAX_ROOTSIGNATRUE_PARAMETERS");

			for (size_t i = 0; i < m_RootSignatureseElements.size(); i++)
			{
				CD3DX12_ROOT_PARAMETER param;
				RootSignatureElement& rs = m_RootSignatureseElements[i];
				switch (rs.slotType)
				{
				case SlotType::OutputView:
					break;
				case SlotType::AccelerationStructure:
					m_GlobalRootParameters[rs.index].InitAsConstants(rs.constantSize, rs.shaderRegister);
					break;
				case SlotType::SceneConstant:

					break;

				default:
					break;
				}
			}
			D3D12_ROOT_SIGNATURE_DESC desc;
			desc.NumParameters = static_cast<UINT>(m_RootSignatureseElements.size());
			desc.pParameters = m_GlobalRootParameters;
			desc.Flags = flags;

			return desc;

		}
	private:
		std::vector<RootSignatureElement> m_RootSignatureseElements;
		CD3DX12_ROOT_PARAMETER m_GlobalRootParameters[MAX_ROOTSIGNATRUE_PARAMETERS];
	};


}
#endif