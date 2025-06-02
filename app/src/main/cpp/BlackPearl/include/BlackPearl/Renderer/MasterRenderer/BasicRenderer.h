#pragma once
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Renderer/DrawStrategy.h"
#include "BlackPearl/Renderer/Material/MaterialBindingCache.h"
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Node/BatchNode.h"
#include "BlackPearl/Node/InstanceNode.h"
#include "BlackPearl/Node/SingleNode.h"
#include "BlackPearl/Node/Node.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Renderer/Shader/ShaderParameters.h"

namespace BlackPearl 
{
	class BasicRenderer
	{
	public:
		BasicRenderer(IDevice* device);
		virtual ~BasicRenderer();
		virtual void RenderScene(const std::vector<Object *>&objs, const LightSources* lightSources,SceneData *scene = Renderer::GetSceneData());
		
		//ÿ��Object��renderǰ������
		virtual void RenderConfigure(Object * obj);
		//TODO::��������������ɾ��
		void DrawObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs, SceneData* scene = Renderer::GetSceneData());
		void DrawObjectsExcept(const std::vector<Object*>& objs, const Object* exceptObj, SceneData* scene = Renderer::GetSceneData());

		//��ͬobjectʹ�ò�ͬshader,ʹ��ǰҪ������shader
		virtual void DrawObjects(std::vector<Object *>objs, SceneData* scene = Renderer::GetSceneData());
		//����Objectsʹ��ͬһ��shader
		virtual void DrawObjects(std::vector<Object *>objs, IShader* shader, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		//ʹ��ǰҪ������shader,���Object�Ĳ�ͬMesh�����в�ͬ��shader
		virtual void DrawObject(Object *obj, SceneData* scene = Renderer::GetSceneData(),unsigned int textureBeginIdx=2);
		//���Object������Meshʹ��ͬһ��shader
		virtual void DrawObject(Object *obj, IShader* shader, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DrawObjectVertex(Object* obj, IShader* shader, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DiscpatchCompute(uint32_t x, uint32_t y, uint32_t z);

		void DrawBatchNode(BatchNode* node, IShader* shader);
		void DrawInstanceNode(InstanceNode* node, IShader* shader);
		void DrawSingleNode(SingleNode* node, const IShader*& shader);
		void DrawMultiIndirect(std::shared_ptr<VertexArray> vertexArray, IShader* shader, uint32_t cmdsCnt);
		void DrawTerrain(Object* obj, IShader* shader, bool drawPolygon = false);

		void DrawPointLight(Object *obj, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void DrawLightSources(const LightSources* lightSources, SceneData* scene = Renderer::GetSceneData(), unsigned int textureBeginIdx = 2);
		void PrepareBasicShaderParameters(std::shared_ptr<class Mesh> mesh,IShader* shader, bool isLight = false, unsigned int textureBeginIdx = 2);
	
	protected:
		void SetupView(ICommandList* commandList, const IView* view, const IView* viewPrev);

        void SetupInputBuffers(ICommandList* cmdList, BufferGroup* buffers, Transform* trans, GraphicsState& state);
        bool SetupMaterial(const Material* material, RasterCullMode cullMode, const GraphicsPipelineDesc& pipelineDesc, GraphicsState& state);
		void RenderPassTemplate(ICommandList* cmdList, IFramebuffer* framebuffer, IView* view, IDrawStrategy* drawStrategy, const ShaderParameters* shaderParms);
		void Draw(ICommandList* cmdList, const DrawItem& item);
		
        void PrepareLights(
            ICommandList* commandList,
            LightSources* lights);



		DeviceHandle m_Device;
		std::shared_ptr<MaterialBindingCache> m_MaterialBindingsCache;

		BufferHandle  m_ForwardViewCB;
		SamplerHandle m_ShadowSampler;
		BufferHandle m_ObjectTransformCB;

		BindingLayoutHandle m_ViewBindinglayout;
		BindingSetHandle m_ViewBindingset;


    private:
        void _UploadIndexBuffers(ICommandList* commandList, BufferGroup* buffers, GraphicsState& state);
        void _UploadVertexBuffers(ICommandList* commandList, BufferGroup* buffers, GraphicsState& state, InputLayoutHandle inputLayout);
        void _UploadInstanceBuffers(ICommandList* commandList, BufferGroup* buffers, Transform* trans, GraphicsState& state);
		void _UploadTransformBuffers(ICommandList* commandList, Transform* trans, GraphicsState& state);


	public:

		static uint32_t s_DrawCallCnt;

	
	};


}
