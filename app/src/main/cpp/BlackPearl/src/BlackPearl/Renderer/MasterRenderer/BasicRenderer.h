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
		~BasicRenderer();
		virtual void RenderScene(const std::vector<Object *>&objs, const LightSources* lightSources,Renderer::SceneData *scene = Renderer::GetSceneData());
		
		//ÿ��Object��renderǰ������
		virtual void RenderConfigure(Object * obj);
		//TODO::��������������ɾ��
		void DrawObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs, Renderer::SceneData* scene = Renderer::GetSceneData());
		void DrawObjectsExcept(const std::vector<Object*>& objs, const Object* exceptObj, Renderer::SceneData* scene = Renderer::GetSceneData());

		//��ͬobjectʹ�ò�ͬshader,ʹ��ǰҪ������shader
		virtual void DrawObjects(std::vector<Object *>objs, Renderer::SceneData* scene = Renderer::GetSceneData());
		//����Objectsʹ��ͬһ��shader
		virtual void DrawObjects(std::vector<Object *>objs,std::shared_ptr<Shader> shader, Renderer::SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		//ʹ��ǰҪ������shader,���Object�Ĳ�ͬMesh�����в�ͬ��shader
		virtual void DrawObject(Object *obj, Renderer::SceneData* scene = Renderer::GetSceneData(),unsigned int textureBeginIdx=2);
		//���Object������Meshʹ��ͬһ��shader
		virtual void DrawObject(Object *obj, std::shared_ptr<Shader> shader, Renderer::SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DrawObjectVertex(Object* obj, std::shared_ptr<Shader> shader, Renderer::SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DiscpatchCompute(uint32_t x, uint32_t y, uint32_t z);

		void DrawBatchNode(BatchNode* node, std::shared_ptr<Shader> shader);
		void DrawInstanceNode(InstanceNode* node, std::shared_ptr<Shader> shader);
		void DrawSingleNode(SingleNode* node, const std::shared_ptr<Shader>& shader);
		void DrawMultiIndirect(std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<Shader> shader, uint32_t cmdsCnt);
		void DrawTerrain(Object* obj, std::shared_ptr<Shader> shader, bool drawPolygon = false);

		void DrawPointLight(Object *obj, Renderer::SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DrawLightSources(const LightSources* lightSources, Renderer::SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void PrepareBasicShaderParameters(std::shared_ptr<Mesh> mesh,std::shared_ptr<Shader> shader, bool isLight = false, unsigned int textureBeginIdx = 2);
	public:
		static uint32_t s_DrawCallCnt;
	
	};


}
