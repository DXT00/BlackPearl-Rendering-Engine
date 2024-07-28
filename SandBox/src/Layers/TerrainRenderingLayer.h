#pragma once
#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class TerrainRenderingLayer :public BlackPearl::Layer {
public:

	TerrainRenderingLayer(const std::string& name)
		: Layer(name)
	{

		/*m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();*/
		m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_MainCamera->GetObj());

		//m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight));

		BlackPearl::Renderer::Init();
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();

		m_SkyBoxObj = CreateSkyBox(
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			});
		//m_ObjectsList.push_back(m_SkyBoxObj);
		//m_PlaneObj =CreatePlane();
		//m_QuadObj = CreateQuad();

		m_CubeObj = CreateCube("assets/shaders/CubeRefraction.glsl", "");
		auto meshComponent = m_CubeObj->GetComponent<BlackPearl::MeshRenderer>();
		/*std::shared_ptr<BlackPearl::Texture> cubeMapTexture(DBG_NEW BlackPearl::CubeMapTexture(BlackPearl::Texture::Type::CubeMap,
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			}, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE));*/


		BlackPearl::TextureDesc desc;
		desc.type = BlackPearl::TextureType::CubeMap;
		desc.minFilter = BlackPearl::FilterMode::Linear;
		desc.magFilter = BlackPearl::FilterMode::Linear;
		desc.wrap = BlackPearl::SamplerAddressMode::ClampToEdge;
		desc.format = BlackPearl::Format::RGB8_UNORM;
		desc.faces = { "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
		};
		BlackPearl::TextureHandle cubeMapTexture = m_DeviceManager->GetDevice()->createTexture(desc);

		meshComponent->SetTexture(0, cubeMapTexture);
		//	m_ObjectsList.push_back(m_CubeObj);

			//把FrameBuffer中的texture作为贴图，贴到m_CubeObj上
		//	auto CubemeshComponent = m_CubeObj->GetComponent<BlackPearl::MeshRenderer>();
			//CubemeshComponent->SetTexture(0, m_FrameBuffer->GetColorTexture());
			//	Layer::CreateLight(BlackPearl::LightType::PointLight);



			// (optional) set browser properties

			//m_fileDialog.SetTypeFilters({ ".h", ".cpp" });
	}

	virtual ~TerrainRenderingLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		/*for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj->HasComponent<BlackPearl::MeshRenderer>())
				obj->GetComponent<BlackPearl::Transform>()->SetRotation({
				obj->GetComponent<BlackPearl::Transform>()->GetRotation().x,
				 sin(glfwGetTime() * 0.5) * 90.0f,
				 obj->GetComponent<BlackPearl::Transform>()->GetRotation().z,

					});
		}*/
		// render
		// Render to our framebuffer
		//m_FrameBuffer->Bind(960, 540);
		//glEnable(GL_DEPTH_TEST);


		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


		//	m_MasterRenderer->RenderSceneExcept(m_ObjectsList, m_SkyBoxObj, GetLightSources());

			//小于等于当前深度缓冲的fragment才被绘制
		glDepthFunc(GL_LEQUAL);
		m_BasicRenderer->DrawObject(m_SkyBoxObj);
		//	m_MasterRenderer->RenderObject(m_SkyBoxObj);

		glDepthFunc(GL_LESS);



	}


	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SkyBoxObj;


	BlackPearl::math::float4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };
	BlackPearl::MasterRenderer* m_MasterRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;

};
