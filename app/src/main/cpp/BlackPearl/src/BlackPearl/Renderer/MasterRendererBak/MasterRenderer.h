#pragma once

#include "BlackPearl/Renderer/MasterRenderer/ShadowMapRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/ShadowMapPointLightRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Object/Object.h"

namespace BlackPearl {


	class MasterRenderer
	{
	public:
		MasterRenderer(Object *camera) {
			m_ShadowMapRenderer = DBG_NEW ShadowMapRenderer(camera);
		};
		~MasterRenderer();
		void RenderScene(const std::vector<Object*>&objs, const LightSources*lightSources);
		void RenderSceneExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs, const LightSources* lightSources);
		void RenderSceneExcept(const std::vector<Object*>& objs, const Object* exceptObj, const LightSources* lightSources);


		void RenderObject(Object* objs);
		void RenderShadowMap(const std::vector<Object*>&objs, ParallelLight* sun, const std::vector<Object*>&exceptObjs = std::vector<Object*>());
		//void RenderPointLightShadowMap(const std::vector<Object*>& objs, Object* pointLight, const std::vector<Object*>& exceptObjs = std::vector<Object*>());
		void RenderObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs);
		void RenderObjectsExcept(const std::vector<Object*>& objs, const Object* exceptObj);


		ShadowMapRenderer GetShadowMapRenderer()const { return *m_ShadowMapRenderer; }
		glm::mat4 GetShadowMapLightProjectionMatrx() const { return m_ShadowMapRenderer->GetLightProjectionViewMatrix(); }
		
		ShadowMapPointLightRenderer GetShadowMapPointLightRenderer() const { return m_ShadowMapPointLightRenderer; }
	private:
		ShadowMapRenderer *m_ShadowMapRenderer;
		BasicRenderer m_BasicRenderer;
		ShadowMapPointLightRenderer m_ShadowMapPointLightRenderer;
		//ParallelLight *m_Sun;
		
	};

}