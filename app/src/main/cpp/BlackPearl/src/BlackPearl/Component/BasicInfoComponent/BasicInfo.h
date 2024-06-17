#pragma once
#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/AABB/AABB.h"
namespace BlackPearl {
	enum ObjectType {
		OT_Group,
		OT_BVH_Node,
		OT_Volume,
		OT_Sphere,
		OT_TriMesh,
		OT_Triangle,
		OT_RTXTransformNode,
		OT_SkyBox,
		OT_Model,
		OT_Cube,
		OT_Plane,
		OT_Light,
		OT_LightProbe,
		OT_Camera,
		OT_Quad,
		OT_Terrain,
		OT_BatchNode,
		Init
	};
	class BasicInfo : public Component<BasicInfo>
	{
	public:

		BasicInfo()
			:Component(Component::Type::BasicInfo) {

			m_Type = Component::Type::BasicInfo;
			m_ObjType = ObjectType::Init;

		}
		ObjectType GetObjectType() const { return m_ObjType; }
		void SetObjectType(ObjectType objType);
	private:
		ObjectType m_ObjType;
	};

}

