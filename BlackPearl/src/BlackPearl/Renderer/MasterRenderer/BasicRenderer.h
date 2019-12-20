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
		
		//每个Object在render前的设置
		void RenderConfigure(Object * obj);
		//TODO::这两个函数可以删掉
		void DrawObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs);
		void DrawObjectsExcept(const std::vector<Object*>& objs, const Object* exceptObj);

		//不同object使用不同shader,使用前要先设置shader
		void DrawObjects(std::vector<Object *>objs);
		//所有Objects使用同一个shader
		void DrawObjects(std::vector<Object *>objs,std::shared_ptr<Shader> shader);
		//使用前要先设置shader,这个Object的不同Mesh可以有不同的shader
		void DrawObject(Object *obj);
		//这个Object的所有Mesh使用同一个shader
		void DrawObject(Object *obj, std::shared_ptr<Shader> shader);

		void DrawPointLight(Object *obj);
		void DrawLightSources(const LightSources* lightSources);
		void PrepareBasicShaderParameters(Mesh &mesh, glm::mat4 transformMatrix,std::shared_ptr<Shader> shader, bool isLight = false);

		virtual void PrepareShaderParameters(Mesh &mesh, glm::mat4 transformMatrix, std::shared_ptr<Shader> shader, bool isLight = false);

	};


}
