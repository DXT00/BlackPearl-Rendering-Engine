#include "pch.h"
#include "MasterRenderer.h"

namespace BlackPearl {


	MasterRenderer::~MasterRenderer()
	{
	}
	void MasterRenderer::RenderScene(const std::vector<Object*>& objs, const LightSources* lightSources)
	{
		m_BasicRenderer.RenderScene(objs, lightSources);// RenderScene(objs, lightSources);
	}
	void MasterRenderer::RenderObject(Object * obj)
	{
		m_BasicRenderer.DrawObject(obj);
	}

	void MasterRenderer::RenderObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs)
	{
		m_BasicRenderer.DrawObjectsExcept(objs, exceptObjs);
	}
	void MasterRenderer::RenderObjectsExcept(const std::vector<Object*>& objs, const Object* exceptObj)
	{
		m_BasicRenderer.DrawObjectsExcept(objs,exceptObj);
	}

	void MasterRenderer::RenderSceneExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs, const LightSources* lightSources)
	{
		m_BasicRenderer.DrawLightSources(lightSources);
		m_BasicRenderer.DrawObjectsExcept(objs, exceptObjs);
	}
	void MasterRenderer::RenderSceneExcept(const std::vector<Object*>& objs, const Object* exceptObj, const LightSources* lightSources)
	{
		m_BasicRenderer.DrawLightSources(lightSources);
		m_BasicRenderer.DrawObjectsExcept(objs, exceptObj);

	}
	void MasterRenderer::RenderShadowMap(const std::vector<Object*>& objs, ParallelLight*sun, const std::vector<Object*>&exceptObjs)
	{
		m_ShadowMapRenderer.Render(objs, sun, exceptObjs);
	}
	void MasterRenderer::RenderPointLightShadowMap(const std::vector<Object*>& objs, Object* pointLight, const std::vector<Object*>& exceptObjs) {
		m_ShadowMapPointLightRenderer.Render(objs, pointLight, exceptObjs);
	}
}