#pragma once
#include "BlackPearl/Renderer/MasterRenderer/IBLRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/SkyboxRenderer.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Renderer/MasterRenderer/AnimatedModelRenderer.h"

namespace BlackPearl {
	
	class IBLProbesRenderer:public BasicRenderer
	{
	public:
		IBLProbesRenderer();
		virtual ~IBLProbesRenderer() {
			/*GE_SAVE_DELETE(m_BrdfLUTQuadObj);
			GE_SAVE_DELETE(m_SHQuadObj);
			GE_SAVE_DELETE(m_ProbeCamera);*/

		}
		void Render(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond,
			const std::vector<Object*> diffuseProbes, 
			const std::vector<Object*> reflectionProbes, Object* skyBox);
		void RenderDiffuseProbeMap(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond,
			const std::vector<Object*> diffuseProbes, Object* skyBox);
		void RenderDiffuseProbeMap(int k,const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond,
			const std::vector<Object*> diffuseProbes, Object* skyBox);
		void RenderSpecularProbeMap(bool sceneChanged, const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond,
			const std::vector<Object*> reflectionProbes, Object* skyBox);
		void RenderSpecularProbeMap( const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond,
			const std::vector<Object*> reflectionProbes, Object* skyBox);
		void RenderProbes(const std::vector<Object*> probes,int probeType);
	//	void RenderSpecularObjects(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> probes);

		void Init(MainCamera* camera, Object* brdfLUTQuadObj, const LightSources& lightSources, const std::vector<Object*> objects);
		std::shared_ptr<Texture> GetSpecularBrdfLUTTexture() const { return m_SpecularBrdfLUTTexture; }
		//void UpdateProbesMaps(const LightSources* lightSources, std::vector<Object*> objects, Object* skyBox, Object* probe);
		
		void ReUpdateProbes() {
			m_CurrentProbeIndex = 0;
			m_UpdateFinished = false;
		}
		void UpdateProbeCamera(Object* probe);
	private:
		void UpdateDiffuseProbesMap(const LightSources* lightSources, std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond, Object* skyBox, Object* diffuseProbe);

		void UpdateReflectionProbesMap(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond, Object* skyBox, Object* reflectionProbe);
		/*render environment CubeMap of each probe */
		TextureHandle RenderEnvironmerntCubeMaps(const LightSources* lightSources, std::vector<Object*> objects, std::vector<Object*> dynamicObjs, float timeInSecond, Object* probes, Object* skyBox);
		//void RenderDiffuseIrradianceMap(const LightSources* lightSources, std::vector<Object*> objects, Object *probe);
		void RenderSpecularPrefilterMap(const LightSources* lightSources,  Object *probe, TextureHandle environmentMap);

		void RenderSpecularBRDFLUTMap();
		void RenderSHImage(Object* probe, TextureHandle environmentMap);
		//std::vector<Object*> FindKnearProbes(math::float3 objPos,const std::vector<Object*> probes);


		//TODO::
		static math::float3 s_ProbeGridOffset;
		static float s_ProbeGridSpace;
		

	private:
		/*只计算一次*/
		bool m_IsRenderSpecularBRDFLUTMap = false;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;

		glm::mat4 m_ProbeProjection;
		std::vector<glm::mat4> m_ProbeView;
		std::vector<glm::mat4> m_ProbeProjectionViews;

		Object* m_BrdfLUTQuadObj = nullptr;
		Object* m_SHQuadObj = nullptr;
		bool m_IsInitial = false;
		/*只计算一次*/
		TextureHandle m_SpecularBrdfLUTTexture = nullptr;

		/*draw lighprobes shader*/
		std::shared_ptr<Shader> m_LightProbeShader = nullptr;
		/*shader*/
		std::shared_ptr<Shader>		m_IBLShader = nullptr; //scene renderer
		std::shared_ptr<Shader>		m_IrradianceShader = nullptr; //create diffuse irradianceCubeMap
		std::shared_ptr<Shader>		m_SpecularPrefilterShader = nullptr; //specular prefilter shader
		std::shared_ptr<Shader>		m_SpecularBRDFLutShader = nullptr;  // brdf LUT shader
		std::shared_ptr<Shader>		m_PbrShader = nullptr;
		std::shared_ptr<Shader>		m_NonPbrShader = nullptr;
		//std::shared_ptr<Shader>		m_SHShader = nullptr;//render SHImage for each probe
		/* m_K near probes for specular objs rendering */
		/*注意 物体blending 多个(m_K个) probes的diffuse SH,但只取最近的一个Probe的specular Map*/
		unsigned int m_K = 1;

		/*light probes update queue,update m_KperFrame pre frame*/
		unsigned m_KperFrame = 6;
		unsigned int m_CurrentProbeIndex = 0;
		bool m_UpdateFinished = false;
		MainCamera* m_ProbeCamera;

		/*renderer environmentMap for dynamic objects*/
		AnimatedModelRenderer* m_AnimatedModelRenderer;
		SkyboxRenderer* m_SkyboxRenderer;

	};

}

