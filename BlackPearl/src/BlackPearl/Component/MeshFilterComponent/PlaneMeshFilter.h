#include "BlackPearl/Component/MeshFilterComponent/MeshFilter.h"
#include "BlackPearl/Entity/Entity.h"
namespace BlackPearl {

	class PlaneMeshFilter : public MeshFilter
	{
	public:
		PlaneMeshFilter(EntityManager * entityManager, Entity::Id id)
			: MeshFilter(entityManager, id) {
			Init();
		};
		
		void Init();
		~PlaneMeshFilter();
	
	};


}
