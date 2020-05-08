#pragma once
#include "BlackPearl/Renderer/MasterRenderer/IBLRenderer.h"
#include "BlackPearl/LightProbes/LightProbes.h"
#include "BlackPearl/Object/Object.h"
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
		void Render(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> diffuseProbes, const std::vector<LightProbe*> reflectionProbes, Object* skyBox);
		void RenderProbes(const std::vector<LightProbe*> probes,int probeType);
		void RenderSpecularObjects(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes);



		void Init(MainCamera* camera, Object* brdfLUTQuadObj, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes);
		std::shared_ptr<Texture> GetSpecularBrdfLUTTexture() const { return m_SpecularBrdfLUTTexture; }
		//void UpdateProbesMaps(const LightSources* lightSources, std::vector<Object*> objects, Object* skyBox, LightProbe* probe);
		void UpdateDiffuseProbesMap(const LightSources* lightSources, std::vector<Object*> objects, Object* skyBox, LightProbe* diffuseProbe);

		void UpdateReflectionProbesMap(const LightSources* lightSources, std::vector<Object*> objects, Object* skyBox, LightProbe* reflectionProbe);
		void ReUpdateProbes() {
			m_CurrentProbeIndex = 0;
			m_UpdateFinished = false;
		}
		void UpdateProbeCamera(LightProbe* probe);
	private:
		/*render environment CubeMap of each probe */
		std::shared_ptr<CubeMapTexture> RenderEnvironmerntCubeMaps(const LightSources* lightSources, std::vector<Object*> objects, LightProbe* probes, Object* skyBox);
		//void RenderDiffuseIrradianceMap(const LightSources* lightSources, std::vector<Object*> objects, LightProbe *probe);
		void RenderSpecularPrefilterMap(const LightSources* lightSources,  LightProbe *probe, std::shared_ptr<CubeMapTexture> environmentMap);

		void RenderSpecularBRDFLUTMap();
		void RenderSHImage(LightProbe* probe, std::shared_ptr<CubeMapTexture> environmentMap);
		std::vector<LightProbe*> FindKnearProbes(glm::vec3 objPos,const std::vector<LightProbe*> probes);

		

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
		std::shared_ptr<Texture> m_SpecularBrdfLUTTexture = nullptr;

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
	};

}

