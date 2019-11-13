#pragma once

#include <string>
#include "Object/Object.h"
#include "ObjectManager/ObjectManager.h"
#include "Component/LightComponent/Light.h"
#include "Component/LightComponent/LightSources.h"
#include "Timestep/Timestep.h"
#include "Component/TransformComponent/Transform.h"
#include "Component/MeshRendererComponent/MeshRenderer.h"
namespace BlackPearl {

	class Layer
	{
	public:
		Layer(const std::string& name, ObjectManager *objectManager)
			:m_DebugName(name), m_ObjectManager(objectManager){
			m_LightSources    = new LightSources();
		

		}
		virtual ~Layer() {
			delete m_LightSources;
			delete m_ObjectManager;
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
		virtual Object* CreateCube();
		virtual Object* CreatePlane();

		virtual Object* CreateQuad();

		void ShowMeshRenderer(std::shared_ptr<BlackPearl::MeshRenderer> comp);
		void ShowTransform(std::shared_ptr<BlackPearl::Transform> comp);
		void ShowLight(std::shared_ptr<BlackPearl::Light>comp);

		std::vector<Object*> GetObjects();
		std::vector<std::string> GetObjectsName();
		void DrawObjects();
		void DrawObject(Object* obj);
		void DrawObjectsExcept(Object* obj);

		void DestroyObjects();


	protected:
		std::string      m_DebugName;

		ObjectManager*   m_ObjectManager;
		LightSources*    m_LightSources;

	};

}