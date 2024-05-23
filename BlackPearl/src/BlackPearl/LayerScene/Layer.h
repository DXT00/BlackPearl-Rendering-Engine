#pragma once
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/ObjectManager/ObjectManager.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
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
#include "BlackPearl/Map/MapManager.h"
#include "BlackPearl/Node/BatchNode.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include <WinUser.h>

using namespace std::chrono;

namespace BlackPearl {
	extern ObjectManager* g_objectManager;

	class Layer
	{
	public:
		Layer(const std::string& name)
			:m_DebugName(name) {

			m_LightSources = new LightSources();
			m_fileDialog.SetTitle("file selector");
			m_fileDialog.SetPwd("./assets");

			/*MainCamera Init*/
			m_MainCamera = CreateCamera();
			m_MainCamera->SetPosition(glm::vec3(0, 0, 0.0f));
			m_CameraPosition = m_MainCamera->GetPosition();
			m_CameraRotation.Yaw = m_MainCamera->Yaw();
			m_CameraRotation.Pitch = m_MainCamera->Pitch();

			/*Render*/
			m_DeviceManager = DeviceManager::Create(DynamicRHI::g_RHIType);

			/*Status*/
			m_StartTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		}
		virtual ~Layer() {

			for (Object* obj : m_ObjectsList) {
				GE_SAVE_DELETE(obj);
			}
			//GE_SAVE_DELETE(obj);
			/*for (auto& obj : m_BackGroundObjsList) //不要m_BackGroundObjsList与m_ObjectsList重复删除！
				GE_SAVE_DELETE(obj);
			for (auto& obj : m_DynamicObjsList)
				GE_SAVE_DELETE(obj);
			for (auto& obj : m_ShadowObjsList)
				GE_SAVE_DELETE(obj);*/

			GE_SAVE_DELETE(m_LightSources);
			GE_SAVE_DELETE(m_MainCamera);
			m_ObjectsList.clear();
			m_BackGroundObjsList.clear();
			m_DynamicObjsList.clear();
			m_ShadowObjsList.clear();
			GE_SAVE_DELETE(m_DeviceManager);

		};
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {


		}
		virtual void OnImguiRender();


		void LoadScene(const std::string demoScene);
		void LoadCornellScene();
		void LoadCornellScene1();

		void LoadChurchScene();
		void LoadSpheresScene();
		void LoadSpheresSpecularProbeScene();

		void LoadSwordScene();
		void LoadCubesScene();
		std::vector<Object*> Layer::LoadCubesScene1(int cubeNum, glm::vec3 pos);
		void LoadD3D12ModelScene();

		Object* LoadDynamicObject(const std::string modelName);
		Object* LoadStaticBackGroundObject(const std::string modelName);

		inline std::string GetString() { return m_DebugName; }

		LightSources* GetLightSources() { return m_LightSources; }

		Object* CreateEmpty(std::string name = "");
		Object* CreateGroup(const std::string name = "Group");
		Object* CreateBVHNode(const std::vector<Object*>& objs, const std::string name = "BVHNode");
		Object* CreateTerrain(const std::string& heightMapPath, const std::string& shaderPath = "", const std::string& texturePath = "", uint32_t chunkCntX = 8, uint32_t chunkCntZ = 8, const std::string name = "Terrain");


		Object* CreateLight(LightType type, const std::string& name = "Light");

		Object* CreateModel(
			const std::string& modelPath,
			const std::string& shaderPath,
			const bool isAnimated,
			const std::string& name = "Model",
			const bool vertices_sorted = false,
			const bool createMeshlet = false,
			const bool isMeshletModel = false,
			MeshletOption options = MeshletOption());
		Object* CreateCube(const std::string& shaderPath = "assets/shaders/Cube.glsl", const std::string& texturePath = "", const std::string& name = "Cube");
		Object* CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath = "assets/shaders/Sphere.glsl", const std::string& texturePath = "", const std::string& name = "Sphere");
		Object* CreatePlane(const std::string& shaderPath = "assets/shaders/Plane.glsl", const std::string& texturePath = "assets/texture/wood.png", const std::string& name = "Plane");
		Object* CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath = "assets/shaders/SkyBox.glsl", const std::string& name = "SkyBox");
		//TODO::Quad 加TexturePath就会出bug...
		Object* CreateQuad(const std::string& shaderPath = "assets/shaders/Quad.glsl", const std::string& texturePath = "", const std::string& name = "Quad");

		Object* CreateLightProbe(ProbeType type, const std::string& shaderPath = "assets/shaders/lightProbes/lightProbe.glsl", const std::string& texturePath = "", const std::string& name = "LightProbe");
		Object* CreateProbeGrid(MapManager* mapManager, ProbeType type, glm::vec3 probeNums, glm::vec3 offsets, float space);
		MainCamera* CreateCamera(const std::string& name = "Camera");
		BatchNode* CreateBatchNode(std::vector<Object*> objs, bool dynamic, const std::string& name = "BatchNode");

		void ShowMeshRenderer(MeshRenderer* comp);
		void ShowTransform(Transform* comp, Object* obj);
		void ShowLightProbe(LightProbe* probe, Object* obj);
		void ShowPointLight(PointLight* pointLight);
		void ShowParallelLight(ParallelLight* parallelLight);
		void ShowCamera(PerspectiveCamera* perspectiveCamera);
		void ShowCamera(MainCamera* mainCamera);
		void ShowTerrian(Object* obj);

		void ShowShader(std::string imguiShaders, int meshIndex, static int& itemIndex, int offset);
		void ShowTextures(std::string imguiShaders, int meshIndex, static  int& itemIndex, Texture::Type textureType, static Texture::Type& type, int offset);
		void ShowMaterialProps(Material::Props& imGuiProps);
		std::vector<Object*> GetObjects();
		//std::vector<std::string> GetObjectsName();


		void DestroyObjects();

		void InputCheck(float ts)
		{
			float maxMoveDelta = 5 * m_MainCamera->GetMoveSpeed();
			float moveDelta = m_MainCamera->GetMoveSpeed() * ts;

			if (moveDelta > maxMoveDelta)
				moveDelta = maxMoveDelta;

			if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_W))) {
				if (DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
					m_CameraPosition -= m_MainCamera->Front() * moveDelta;
				}
				else {
					m_CameraPosition += m_MainCamera->Front() * moveDelta;
				}
			}
			else if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_S))) {
				if (DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
					m_CameraPosition += m_MainCamera->Front() * moveDelta;
				}
				else {
					m_CameraPosition -= m_MainCamera->Front() * moveDelta;
				}
			}
			if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_A))) {
				if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
					m_CameraPosition -= (-m_MainCamera->Right()) * moveDelta;
				}
				else if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL ||
					DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
					m_CameraPosition -= m_MainCamera->Right() * moveDelta;
				}
			}
			else if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_D))) {
				if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
					m_CameraPosition += (-m_MainCamera->Right()) * moveDelta;
				}
				else if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL ||
					DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
					m_CameraPosition += m_MainCamera->Right() * moveDelta;
				}
			}
			if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_E))) {
				m_CameraPosition += m_MainCamera->Up() * moveDelta;
			}
			else if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_Q))) {
				m_CameraPosition -= m_MainCamera->Up() * moveDelta;
			}
			// ---------------------Rotation--------------------------------------

			float posx = Input::GetMouseX();
			float posy = Input::GetMouseY();
			if (Input::IsMouseButtonPressed(KeyCodes::Get(BP_MOUSE_BUTTON_RIGHT))) {
				if (Input::IsFirstMouse()) {
					Input::SetFirstMouse(false);
					m_LastMouseX = posx;
					m_LastMouseY = posy;
				}
				float diffx = posx - m_LastMouseX;
				float diffy = -posy + m_LastMouseY;

				m_LastMouseX = posx;
				m_LastMouseY = posy;

				float deltaX = diffx * ts * m_MainCamera->GetRotateSpeed();
				float deltaY = diffy * ts * m_MainCamera->GetRotateSpeed();

				float maxRotDelta = 3 * m_MainCamera->GetRotateSpeed();
				if (deltaX > maxRotDelta)
					deltaX = maxRotDelta;
				if (deltaY > maxRotDelta)
					deltaY = maxRotDelta;

				//GE_CORE_INFO("Cam deltaX = " + std::to_string(deltaX) + "Cam deltaY =" + std::to_string(deltaY));

				float raoteSpeed = m_MainCamera->GetRotateSpeed();
				m_CameraRotation.Yaw += deltaX ;
				m_CameraRotation.Pitch += deltaY ;

				if (m_CameraRotation.Pitch > 89.0f)
					m_CameraRotation.Pitch = 89.0f;
				if (m_CameraRotation.Pitch < -89.0f)
					m_CameraRotation.Pitch = -89.0f;

				if (m_CameraRotation.Yaw > 0.0f)
					m_CameraRotation.Yaw = 0.0f;
				if (m_CameraRotation.Yaw < -360.0f)
					m_CameraRotation.Yaw = -360.0f;

				m_MainCamera->SetRotation({ m_CameraRotation.Pitch, m_CameraRotation.Yaw, 0.0f });
			}
			else {
				m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
				m_LastMouseY = posy;
			}

			//GE_CORE_INFO("Cam Pitch = " + std::to_string(m_CameraRotation.Pitch) + "Cam Yaw =" + std::to_string(m_CameraRotation.Yaw));
			m_MainCamera->SetPosition(m_CameraPosition);
		}

	protected:
		std::string      m_DebugName;

		//每层Layer都有一个ObjectsList
		std::vector<Object*>          m_ObjectsList;
		/*用于lightProbes 作为背景的Objects*/
		std::vector<Object*> m_BackGroundObjsList;
		std::vector<Object*> m_DynamicObjsList;
		std::vector<Object*> m_ShadowObjsList;


		LightSources* m_LightSources;
		ImGui::FileBrowser m_fileDialog;
		glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };

		/*MainCamera and Input*/

		MainCamera* m_MainCamera = nullptr;
		glm::vec3 m_CameraPosition = { 0.0f,0.0f,5.0f };
		struct CameraRotation {
			float Yaw;
			float Pitch;

		};
		CameraRotation m_CameraRotation;
		float m_LastMouseX;
		float m_LastMouseY;


		/*Time*/
		std::chrono::milliseconds m_StartTimeMs;

		/* Probes */
		std::vector<Object*> m_DiffuseLightProbes;
		std::vector<Object*> m_ReflectionLightProbes;

		/*Render */
		DeviceManager* m_DeviceManager;
	};

}