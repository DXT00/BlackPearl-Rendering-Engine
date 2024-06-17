#pragma once
#include"BlackPearl/Renderer/VertexArray.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Component/CameraComponent/Camera.h"
namespace BlackPearl {

	class Renderer
	{
	public:
		struct SceneData {
			glm::mat4 ProjectionViewMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;
			glm::vec3 CameraPosition;
			glm::vec3 CameraRotation;

			glm::vec3 CameraFront;
			LightSources LightSources;
		};

		Renderer();
		~Renderer();
		static void Init();
		static void BeginScene(const Camera& camera, const LightSources& lightSources);//每次Update都要调用BeginScene一次，因为Camera的ViewProjection Matrix会改变
		/*默认 scene 是 default camera*/
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4& model = glm::mat4(1.0f), SceneData* sceneData= GetSceneData());//Submmit前记得调用 BeginScene()!
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, float* model, uint32_t objCnt, SceneData* sceneData = GetSceneData());//Submmit前记得调用 BeginScene()!
		void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, SceneData* sceneData);

		static SceneData* GetSceneData() { return s_SceneData; }
	private:

		static SceneData* s_SceneData;
	};

}