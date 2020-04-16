#pragma once

#include <string>

#include "BlackPearl/Object/Object.h"
#include "BlackPearl/ObjectManager/ObjectManager.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Timestep/Timestep.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Input.h"
#include "BlackPearl/KeyCodes.h"
#include <chrono>
using namespace std::chrono;

namespace BlackPearl {

	class Layer
	{
	public:
		Layer(const std::string& name, ObjectManager* objectManager)
			:m_DebugName(name), m_ObjectManager(objectManager) {

			m_LightSources = new LightSources();
			m_fileDialog.SetTitle("file selector");
			m_fileDialog.SetPwd("./assets");

			/*MainCamera Init*/
			m_MainCamera = CreateCamera();
			//m_MainCamera->SetPosition(glm::vec3(0.0f, 1.387f, 8.012f));
			m_MainCamera->SetPosition(glm::vec3(0.0f, 1.387f, 13.012f));

			//m_MainCamera->SetRotation(glm::vec3(-26.5f,-60.8f,0.0f));

			//m_MainCamera->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));

			m_CameraPosition = m_MainCamera->GetPosition();//cameraComponent->GetPosition();
			m_CameraRotation.Yaw = m_MainCamera->Yaw();
			m_CameraRotation.Pitch = m_MainCamera->Pitch();

			m_StartTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		}
		virtual ~Layer() {
			delete m_LightSources;
			delete m_ObjectManager;
			for (Object* obj : m_ObjectsList) {
				if(obj!=nullptr)
					delete obj;

			}
		};
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {
		
			
		}
		virtual void OnImguiRender();


		void LoadScene(const std::string demoScene);
		void LoadCornellScene();
		void LoadSpheresScene();
		void LoadSwordScene();
		void LoadCubesScene();
		Object* LoadDynamicObject(const std::string modelName);
		Object* LoadStaticBackGroundObject(const std::string modelName);

		inline std::string GetString() { return m_DebugName; }

		LightSources* GetLightSources() { return m_LightSources; }

		virtual Object* CreateEmpty(std::string name = "");
		virtual Object* CreateLight(LightType type, const std::string& name = "Light");

		virtual Object* CreateModel(const std::string& modelPath, const std::string& shaderPath,const bool isAnimated,const std::string& name = "Model");
		virtual Object* CreateCube(const std::string& shaderPath = "assets/shaders/Cube.glsl", const std::string& texturePath = "", const std::string& name = "Cube");
		virtual Object* CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath = "assets/shaders/Sphere.glsl", const std::string& texturePath = "", const std::string& name = "Sphere");
		virtual Object* CreatePlane(const std::string& shaderPath = "assets/shaders/Plane.glsl", const std::string& texturePath = "assets/texture/wood.png", const std::string& name = "Plane");
		virtual Object* CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath = "assets/shaders/SkyBox.glsl", const std::string& name = "SkyBox");
		//TODO::Quad 加TexturePath就会出bug...
		virtual Object* CreateQuad(const std::string& shaderPath = "assets/shaders/Quad.glsl", const std::string& texturePath = "", const std::string& name = "Quad");

		virtual LightProbe* CreateLightProbe(const std::string& shaderPath = "assets/shaders/lightProbes/lightProbe.glsl", const std::string& texturePath = "", const std::string& name = "LightProbe");
		virtual MainCamera* CreateCamera(const std::string& name="Camera");

		void ShowMeshRenderer(MeshRenderer* comp);
		void ShowTransform(Transform* comp);
		void ShowPointLight(PointLight* pointLight);
		void ShowCamera(PerspectiveCamera* perspectiveCamera);
		void ShowShader(std::string imguiShaders, int meshIndex, static int& itemIndex, int offset);
		void ShowTextures(std::string imguiShaders, int meshIndex, static  int& itemIndex, Texture::Type textureType, static Texture::Type& type, int offset);
		void ShowMaterialProps(Material::Props& imGuiProps);
		std::vector<Object*> GetObjects();
		std::vector<std::string> GetObjectsName();


		void DestroyObjects();

		void InputCheck(float ts)
		{

			//auto cameraComponent = m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>();
			if (Input::IsKeyPressed(BP_KEY_W)) {
				m_CameraPosition += m_MainCamera->Front() * m_CameraMoveSpeed * ts;
			}
			else if (Input::IsKeyPressed(BP_KEY_S)) {
				m_CameraPosition -= m_MainCamera->Front() * m_CameraMoveSpeed * ts;
			}
			if (Input::IsKeyPressed(BP_KEY_A)) {
				m_CameraPosition -= m_MainCamera->Right() * m_CameraMoveSpeed * ts;
			}
			else if (Input::IsKeyPressed(BP_KEY_D)) {
				m_CameraPosition += m_MainCamera->Right() * m_CameraMoveSpeed * ts;
			}
			if (Input::IsKeyPressed(BP_KEY_E)) {
				m_CameraPosition += m_MainCamera->Up() * m_CameraMoveSpeed * ts;
			}
			else if (Input::IsKeyPressed(BP_KEY_Q)) {
				m_CameraPosition -= m_MainCamera->Up() * m_CameraMoveSpeed * ts;
			}
			// ---------------------Rotation--------------------------------------

			float posx = Input::GetMouseX();
			float posy = Input::GetMouseY();
			if (Input::IsMouseButtonPressed(BP_MOUSE_BUTTON_RIGHT)) {


				if (Input::IsFirstMouse()) {
					Input::SetFirstMouse(false);
					m_LastMouseX = posx;
					m_LastMouseY = posy;
				}
				float diffx = posx - m_LastMouseX;
				float diffy = -posy + m_LastMouseY;

				m_LastMouseX = posx;
				m_LastMouseY = posy;
				m_CameraRotation.Yaw += diffx * m_CameraRotateSpeed * ts;
				m_CameraRotation.Pitch += diffy * m_CameraRotateSpeed * ts;

				if (m_CameraRotation.Pitch > 89.0f)
					m_CameraRotation.Pitch = 89.0f;
				if (m_CameraRotation.Pitch < -89.0f)
					m_CameraRotation.Pitch = -89.0f;

				m_MainCamera->SetRotation({ m_CameraRotation.Pitch,m_CameraRotation.Yaw,0.0f });

			}
			else {

				m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
				m_LastMouseY = posy;
			}


			m_MainCamera->SetPosition(m_CameraPosition);
		}

	protected:
		std::string      m_DebugName;

		ObjectManager* m_ObjectManager;
		//每层Layer都有一个ObjectsList
		std::vector<Object*>          m_ObjectsList;
		/*用于lightProbes 作为背景的Objects*/
		std::vector<BlackPearl::Object*> m_BackGroundObjsList;
		std::vector<BlackPearl::Object*> m_DynamicObjsList;
		std::vector<BlackPearl::Object*> m_ShadowObjsList;


		LightSources* m_LightSources;
		ImGui::FileBrowser m_fileDialog;
		glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };

		/*MainCamera and Input*/

		MainCamera* m_MainCamera = nullptr;
		glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
		struct CameraRotation {
			float Yaw;
			float Pitch;

		};
		CameraRotation m_CameraRotation;
		float m_LastMouseX;
		float m_LastMouseY;
		float m_CameraMoveSpeed = 1.0f;
		float m_CameraRotateSpeed = 1.0f;

		/*Time*/
		std::chrono::milliseconds m_StartTimeMs;
	
		

	};

}