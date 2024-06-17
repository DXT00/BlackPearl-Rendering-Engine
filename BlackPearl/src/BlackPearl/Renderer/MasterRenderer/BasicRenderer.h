#pragma once
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Node/BatchNode.h"
#include "BlackPearl/Node/InstanceNode.h"
#include "BlackPearl/Node/SingleNode.h"
#include "BlackPearl/Node/Node.h"

namespace BlackPearl {
	class BasicRenderer
	{
	public:
		BasicRenderer();
		virtual ~BasicRenderer();
		virtual void RenderScene(const std::vector<Object *>&objs, const LightSources* lightSources,SceneData *scene = Renderer::GetSceneData());
		
		//每个Object在render前的设置
		virtual void RenderConfigure(Object * obj);
		//TODO::这两个函数可以删掉
		void DrawObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs, SceneData* scene = Renderer::GetSceneData());
		void DrawObjectsExcept(const std::vector<Object*>& objs, const Object* exceptObj, SceneData* scene = Renderer::GetSceneData());

		//不同object使用不同shader,使用前要先设置shader
		virtual void DrawObjects(std::vector<Object *>objs, SceneData* scene = Renderer::GetSceneData());
		//所有Objects使用同一个shader
		virtual void DrawObjects(std::vector<Object *>objs,std::shared_ptr<Shader> shader, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		//使用前要先设置shader,这个Object的不同Mesh可以有不同的shader
		virtual void DrawObject(Object *obj, SceneData* scene = Renderer::GetSceneData(),unsigned int textureBeginIdx=2);
		//这个Object的所有Mesh使用同一个shader
		virtual void DrawObject(Object *obj, std::shared_ptr<Shader> shader, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DrawObjectVertex(Object* obj, std::shared_ptr<Shader> shader, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DiscpatchCompute(uint32_t x, uint32_t y, uint32_t z);

		void DrawBatchNode(BatchNode* node, std::shared_ptr<Shader> shader);
		void DrawInstanceNode(InstanceNode* node, std::shared_ptr<Shader> shader);
		void DrawSingleNode(SingleNode* node, const std::shared_ptr<Shader>& shader);
		void DrawMultiIndirect(std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<Shader> shader, uint32_t cmdsCnt);
		void DrawTerrain(Object* obj, std::shared_ptr<Shader> shader, bool drawPolygon = false);

		void DrawPointLight(Object *obj, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DrawLightSources(const LightSources* lightSources, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void PrepareBasicShaderParameters(std::shared_ptr<Mesh> mesh,std::shared_ptr<Shader> shader, bool isLight = false, unsigned int textureBeginIdx = 2);
	public:
		static uint32_t s_DrawCallCnt;
	
	};


}
