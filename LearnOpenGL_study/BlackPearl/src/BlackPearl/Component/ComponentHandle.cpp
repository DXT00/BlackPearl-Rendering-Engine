#include "pch.h"
#include "ComponentHandle.h"

namespace BlackPearl {
	template<typename C>
	inline Entity ComponentHandle<C>::Entity()
	{
		return m_EntityManager->GetEntity(m_Id);
	}

}