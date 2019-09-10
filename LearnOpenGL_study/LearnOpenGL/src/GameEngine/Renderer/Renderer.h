#pragma once
#include"GameEngine/Renderer/VertexArray.h"
#include "GameEngine/Renderer/Shader.h"
#include "GameEngine/Renderer/Camera/Camera.h"
class Renderer
{
public:
	Renderer();
	~Renderer();
	static void Init();
	static void BeginScene(const Camera& camera);//每次Update都要调用BeginScene一次，因为Camera的ViewProjection Matrix会改变
	static void Submit(const std::shared_ptr<VertexArray>& vertexArray,const std::shared_ptr<Shader>& shader,const glm::mat4 &model = glm::mat4(1.0f));//Submmit前记得调用 BeginScene()!
private:
	struct SceneData {
		glm::mat4 ViewProjectionMatrix;
	};
	
	static SceneData* m_SceneData;
};

