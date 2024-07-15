#pragma once
#include"BlackPearl/Renderer/VertexArray.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Component/CameraComponent/Camera.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/Math/frustum.h"
#include "BlackPearl/Math/Math.h"
namespace BlackPearl {

	class IView {
	public:
		[[nodiscard]] virtual ViewportState GetViewportState() const = 0;
		[[nodiscard]] virtual VariableRateShadingState GetVariableRateShadingState() const = 0;

		[[nodiscard]] virtual math::frustum GetViewFrustum() const = 0;
		

	};
	class SceneData : public IView
	{
	public:
		SceneData()
			: ProjectionViewMatrix(glm::mat4(1.0)),
			ViewMatrix(glm::mat4(1.0)),
			ProjectionMatrix(glm::mat4(1.0)),
			CameraPosition(glm::vec3(0.0)),
			CameraRotation(glm::vec3(0.0)),
			CameraFront(glm::vec3(0.0))
			{
				ViewFrustum = math::frustum(Math::ToFloat4x4(ViewMatrix* ProjectionMatrix), ReverseZ);
		    }

		SceneData(const glm::mat4& pvMat,
			const glm::mat4& vMat,
			const glm::mat4& pMat,
			const glm::vec3& camPos,
			const glm::vec3& camRot,
			const glm::vec3& camFront,
			const LightSources& lightSource)
			:ProjectionViewMatrix(pvMat),
			ViewMatrix(vMat),
			ProjectionMatrix(pMat),
			CameraPosition(camPos),
			CameraRotation(camRot),
			CameraFront(camFront),
			LightSources(lightSource)
		{
			ViewFrustum = math::frustum(Math::ToFloat4x4(ViewMatrix * ProjectionMatrix), ReverseZ);

		}


		glm::mat4 ProjectionViewMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		glm::vec3 CameraPosition;
		glm::vec3 CameraRotation;

		glm::vec3 CameraFront;
		LightSources LightSources;

		virtual ViewportState GetViewportState() const override;
		virtual VariableRateShadingState GetVariableRateShadingState() const override;
		virtual math::frustum GetViewFrustum() const override;
		RHIViewport m_Viewport;
		RHIRect m_ScissorRect;
		VariableRateShadingState m_ShadingRateState;

		math::frustum ViewFrustum = math::frustum::empty();
		//TODO::
		bool ReverseZ = false;
	};

	class Renderer
	{
	public:
		

		Renderer();
		~Renderer();
		static void Init();
		static void BeginScene(const Camera& camera, const LightSources& lightSources);//每次Update都要调用BeginScene一次，因为Camera的ViewProjection Matrix会改变
		/*默认 scene 是 default camera*/
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4& model = glm::mat4(1.0f), SceneData* sceneData= GetSceneData());//Submmit前记得调用 BeginScene()!
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, float* model, uint32_t objCnt, SceneData* sceneData = GetSceneData());//Submmit前记得调用 BeginScene()!
		void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, SceneData* sceneData);

		static SceneData* GetSceneData() { return s_SceneData; }
		static SceneData* GetPreSceneData() { return s_PreSceneData; }

	private:

		static SceneData* s_SceneData;
		static SceneData* s_PreSceneData;
	};

}