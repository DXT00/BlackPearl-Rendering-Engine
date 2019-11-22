#pragma once

#include <string>

#include "Object/Object.h"
#include "ObjectManager/ObjectManager.h"
#include "Component/LightComponent/Light.h"
#include "Component/LightComponent/LightSources.h"
#include "Timestep/Timestep.h"
#include "Component/TransformComponent/Transform.h"
#include "Component/MeshRendererComponent/MeshRenderer.h"
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"

namespace BlackPearl {

	class Layer
	{
	public:
		Layer(const std::string& name, ObjectManager *objectManager)
			:m_DebugName(name), m_ObjectManager(objectManager){
			
			m_LightSources    = new LightSources();	
			m_fileDialog.SetTitle("file selector");
			m_fileDialog.SetPwd("./assets");

		}
		virtual ~Layer() {
			delete m_LightSources;
			delete m_ObjectManager;
			for (Object* obj : m_ObjectsList)
				delete obj;
		};
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImguiRender() {}

		inline std::string GetString() { return m_DebugName; }
		
		LightSources *GetLightSources() { return m_LightSources; }

		virtual Object* CreateEmpty(std::string name = "");
		virtual Object* CreateLight(LightType type);
		virtual Object* CreateCamera();

		virtual Object* CreateModel(const std::string& modelPath, const std::string& shaderPath);
		virtual Object* CreateCube(const std::string& shaderPath = "assets/shaders/Cube.glsl", const std::string& texturePath = "assets/texture/wood.png");
		virtual Object* CreatePlane(const std::string& shaderPath = "assets/shaders/Plane.glsl", const std::string& texturePath = "assets/texture/container.jpg");
		virtual Object* CreateSkyBox(const std::vector<std::string>& textureFaces);
		virtual Object* CreateQuad(const std::string& shaderPath = "assets/shaders/Quad.glsl", const std::string& texturePath = "assets/texture/1.jpg");

		void ShowMeshRenderer(std::shared_ptr<BlackPearl::MeshRenderer> comp);
		void ShowTransform(std::shared_ptr<BlackPearl::Transform> comp);
		void ShowLight(std::shared_ptr<BlackPearl::Light>comp);

		void ShowShader( std::string imguiShaders, int meshIndex, static int &itemIndex, int offset);
		void ShowTextures( std::string imguiShaders, int meshIndex, static  int &itemIndex, Texture::Type textureType, static Texture::Type &type,int offset);
		void ShowMaterialProps(Material::Props& imGuiProps);
		std::vector<Object*> GetObjects();
		std::vector<std::string> GetObjectsName();
		void DrawObjects();
		/*void DrawObject(Object* obj);
		void DrawObjectsExcept(std::vector<Object *>objs);
		void DrawObjectsExcept(Object * obj);*/

		void DestroyObjects();


	protected:
		std::string      m_DebugName;

		ObjectManager*   m_ObjectManager;
		//每层Layer都有一个ObjectsList
		std::vector<Object*>          m_ObjectsList;
		LightSources*    m_LightSources;
		ImGui::FileBrowser m_fileDialog;

	};

}