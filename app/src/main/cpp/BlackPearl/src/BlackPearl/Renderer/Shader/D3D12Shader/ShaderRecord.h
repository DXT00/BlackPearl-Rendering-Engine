#pragma once
namespace BlackPearl {
	//ShaderRecord={{Shader ID},{RootArguments}}
	class ShaderRecord
	{
	public:
		ShaderRecord(void* pShaderIdentifier, unsigned int shaderIndentifierSize) {
			m_ShaderIdentifier.ptr = pShaderIdentifier;
			m_ShaderIdentifier.size = shaderIndentifierSize;
		}
		ShaderRecord(void* pShaderIdentifier, unsigned int shaderIndentifierSize,
			void* pRootArgument, unsigned int rootArgumentSize) {
			m_ShaderIdentifier.ptr = pShaderIdentifier;
			m_ShaderIdentifier.size = shaderIndentifierSize;
			if (pRootArgument) {
				m_RootArgument.ptr = pRootArgument;
				m_RootArgument.size = rootArgumentSize;
			}
		}

		void CopyTo(void* dest) const {
			uint8_t* byteDest = static_cast<uint8_t*>(dest);
			memcpy(byteDest, m_ShaderIdentifier.ptr, m_ShaderIdentifier.size);
			if (m_RootArgument.ptr) {
				memcpy(byteDest + m_ShaderIdentifier.size, m_RootArgument.ptr, m_RootArgument.size);
			}
		}
		struct PointerWithSize
		{
			PointerWithSize(void* ptr, unsigned int size)
				:ptr(ptr), size(size){}
			PointerWithSize()
			:ptr(nullptr), size(0){}

			void* ptr;
			unsigned int size;

		};
		PointerWithSize m_ShaderIdentifier;
		PointerWithSize m_RootArgument;
	};

}

