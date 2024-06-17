#include "BlackPearl/Component/MeshFilterComponent/MeshFilter.h"
#include "BlackPearl/Entity/Entity.h"
namespace BlackPearl {

	class PlaneMeshFilter : public MeshFilter
	{
	public:
		PlaneMeshFilter()
			: MeshFilter() {
			Init();
		};
		
		void Init();
		~PlaneMeshFilter();
	
	};


}
