//#pragma once
//#include "Joint.h"
//#include "BlackPearl/Renderer/Model/Model.h"
//#include "BlackPearl/Animation/Animation.h"
//namespace BlackPearl {
//	class AnimatedModel:public Model
//	{
//	public:
///*max number of joints that can affect a vertex*/
//#define MAX_WEIGHT 3
//		struct VertexBoneData {
//			std::vector<unsigned int> jointIdx;
//			std::vector<float> weights;
//			std::vector<glm::mat4> transformToParent;
//		};
//		AnimatedModel(const std::string& path, const std::shared_ptr<Shader>shader)
//			:Model(path, shader) {
//			m_BoneDatas.assign(GetVertiesNum(), VertexBoneData());
//
//		}
//		glm::mat4 AiMatrix4ToMat4(aiMatrix4x4 aiMatrix);
//
//		virtual void LoadModel(const std::string& path) override;
//
//		void LoadBones(aiMesh* aimesh);
//
//		//load animation from assimp
//		void LoadAnimation(const aiScene* scene);
//
//		void CalculateBoneTransform(float timeInSecond,std::vector<glm::mat4>& transform);
//	private:
//		Joint m_RootJoint;
//		unsigned int m_BoneCount;
//		std::vector<VertexBoneData> m_BoneDatas;
//		/*´æ´¢¶à¸ö¶¯»­Ö¡*/
//		std::vector<Animation> m_Animations;
//	
//
//	};
//}
