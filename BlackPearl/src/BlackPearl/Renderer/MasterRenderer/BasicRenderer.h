#pragma once
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
namespace BlackPearl {
	class BasicRenderer
	{
	public:
		BasicRenderer();
		~BasicRenderer();
		void RenderScene(const std::vector<Object *>&objs, const LightSources* lightSources);

		void RenderConfigure(Object * obj);//每个Object在render前的设置
		void DrawObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs);
		void DrawObjectsExcept(const std::vector<Object*>& objs, const Object* exceptObj);

		void DrawObjects(std::vector<Object *>objs);
		void DrawObject(Object *obj);
		void DrawPointLight(Object *obj);
		void DrawLightSources(const LightSources* lightSources);
		void PrepareBasicShaderParameters(Mesh &mesh, glm::mat4 transformMatrix, bool isLight = false);
		virtual void PrepareShaderParameters(Mesh &mesh, glm::mat4 transformMatrix, bool isLight = false);

	};


}
