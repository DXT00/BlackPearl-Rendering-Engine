### Assimp load Animation

reference website:

https://stackoverflow.com/questions/52248731/trying-to-load-animations-in-opengl-from-an-md5-file-using-assimp-glm

http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html

OpenGL Skeletal Animation Tutorial #2
https://www.youtube.com/watch?v=F-kcaonjHf8&list=PLRIWtICgwaX2tKWCxdeB7Wv_rTET9JtWW&index=2


#### 1.assimp导入的 aiMatrix4x4是行对齐矩阵与glm::mat4的列对齐矩阵不一致，所以在计算bone.finalTransform时记得要转置aiMatrix矩阵！

note:
Assimp matrix is row major, so transforming is just a simply copy one element by one element.But GLM and OpenGL matrix are both Column major.

Model.cpp:

```

	std::vector<glm::mat4> Model::CalculateBoneTransform(float timeInSecond)
	{

			unsigned int animationNum = m_Scene->mNumAnimations;
		aiAnimation* animation = m_Scene->mAnimations[0];
		float durationTick = (float)animation->mDuration;
		float tickPerSecond = (float)animation->mTicksPerSecond;
		if (tickPerSecond == 0.0) {
			tickPerSecond = 25.0f;
		}
		float timeInTick = timeInSecond * tickPerSecond;
			float timeInDurationSecond = fmod(timeInTick, durationTick);
		aiMatrix4x4 indentity{
		1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		0.0,0.0,0.0,1.0	
		};
		ReadHierarchy(timeInDurationSecond, m_Scene->mRootNode, indentity);

		std::vector<glm::mat4> boneFinalTransforms;
		for (int i = 0; i < m_BoneCount; i++)
		{
			boneFinalTransforms.push_back(m_Bones[i].finalTransform);
		}

		return boneFinalTransforms;

	}
	void Model::ReadHierarchy(float timeInDurationSecond, aiNode* node, aiMatrix4x4 parentTransform)
	{
		std::string nodeName(node->mName.data);
		aiAnimation* animation = m_Scene->mAnimations[0];
		aiNodeAnim* nodeAnim = FindNode(nodeName, animation);
		aiMatrix4x4 transform = (node->mTransformation);
		

		if (nodeAnim) {

			unsigned int positionNum = nodeAnim->mNumPositionKeys;
			unsigned int rotationNum = nodeAnim->mNumRotationKeys;
			unsigned int scaleNum = nodeAnim->mNumScalingKeys;
			glm::vec3 position = CalculateInterpolatePosition(timeInDurationSecond, nodeAnim);
			aiQuaternion quaternion = CalculateInterpolateRotation(timeInDurationSecond, nodeAnim);
			glm::vec3 scale = CalculateInterpolateScale(timeInDurationSecond, nodeAnim);


			
			aiMatrix3x3 rotateM = (quaternion.GetMatrix());
		
			transform = aiMatrix4x4(rotateM);
			transform.a4 = position.x; transform.b4 = position.y; transform.c4 = position.z; transform.d4 = 1.0f;


		}
		aiMatrix4x4 globalTransform = parentTransform * transform  ;
		if (m_BoneNameToIdex.find(nodeName) != m_BoneNameToIdex.end()) {
			int boneIdex = m_BoneNameToIdex[nodeName];
			aiMatrix4x4 aiFinalTransform = m_GlobalInverseTransform * globalTransform * m_Bones[boneIdex].meshToBoneTranform;
	
			m_Bones[boneIdex].finalTransform = glm::transpose(AiMatrix4ToMat4(aiFinalTransform));
			glm::mat4 temp = glm::mat4(
				glm::vec4(aiFinalTransform.a1,aiFinalTransform.a2, aiFinalTransform.a3, aiFinalTransform.a4),
				glm::vec4(aiFinalTransform.b1, aiFinalTransform.b2, aiFinalTransform.b3, aiFinalTransform.b4),
				glm::vec4(aiFinalTransform.c1, aiFinalTransform.c2, aiFinalTransform.c3, aiFinalTransform.c4),
				glm::vec4(aiFinalTransform.d1, aiFinalTransform.d2, aiFinalTransform.d3, aiFinalTransform.d4)
			);
			glm::mat4 a(1.0f);
		}
		else {
			GE_CORE_ERROR("no such bone" + nodeName);
		}

		for (int i = 0; i < node->mNumChildren; i++)
		{
			ReadHierarchy(timeInDurationSecond, node->mChildren[i], globalTransform);
		}



	}


```

#### 2.修复texture上下颠倒bug

//aiProcess_FlipUVs将在处理的时候翻转y轴的纹理坐标（在OpenGL中大部分的图像的y轴都是反的，所以这个后期处理选项将会修复这个）

这里的Animation Model 我们不需要aiProcess_FlipUVs！！

```
void Model::LoadModel(const std::string& path)
	{
		m_Path = path;
		m_Scene = m_Importer.ReadFile(path,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals);

		//不要加aiProcess_FlipUVs！！！，否则纹理会反！！！
			

		if (!m_Scene || m_Scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode) {
			GE_CORE_ERROR("ERROR::ASSIMP:: {0}", m_Importer.GetErrorString())
				return;
		}
		m_Directory = path.substr(0, path.find_last_of('/'));

		for (int i = 0; i < m_Scene->mNumMeshes; i++)
		{
			m_VerticesNum += m_Scene->mMeshes[i]->mNumVertices;
		}
		m_GlobalInverseTransform = glm::inverse(ConvertMatrix(m_Scene->mRootNode->mTransformation));


		m_BoneDatas.resize(m_VerticesNum);

		for (int i = 0; i < m_Scene->mNumMeshes; i++)
		{
			aiMesh* mesh = m_Scene->mMeshes[i];
			m_Meshes.push_back(ProcessMesh(mesh));
		}
		
	}


```
![Animation](/results/Animation1.png)
![Animation](/results/Animation2.png)



