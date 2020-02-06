#pragma once
#include "BlackPearl/Renderer/MasterRenderer/IBLRenderer.h"
#include "BlackPearl/LightProbes/LightProbes.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {
	
	class IBLProbesRenderer:public BasicRenderer
	{
	public:
		IBLProbesRenderer();
		virtual ~IBLProbesRenderer();
		void Render(const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes);
		void RenderProbes(const std::vector<LightProbe*> probes);
		void RenderSpecularObjects(const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes);
		
		void Init( Object* brdfLUTQuadObj, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes);
		std::shared_ptr<Texture> GetSpecularBrdfLUTTexture() const { return m_SpecularBrdfLUTTexture; }

	private:
		/*render environment CubeMap of each probe */
		void RenderEnvironmerntCubeMaps(const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probes);
		void RenderDiffuseIrradianceMap(const LightSources& lightSources, std::vector<Object*> objects, LightProbe *probe);
		void RenderSpecularPrefilterMap(const LightSources& lightSources, std::vector<Object*> objects, LightProbe *probe);

		void RenderSpecularBRDFLUTMap();

		std::vector<LightProbe*> FindKnearProbes(glm::vec3 objPos,const std::vector<LightProbe*> probes);

	private:
		/*只计算一次*/
		bool m_IsRenderSpecularBRDFLUTMap = false;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;

		glm::mat4 m_ProbeProjection;
		std::vector<glm::mat4> m_ProbeView;
		std::vector<glm::mat4> m_ProbeProjectionViews;

		Object* m_BrdfLUTQuadObj = nullptr;
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

		/* m_K near probes for specular objs rendering */
		unsigned int m_K = 2;
	};

}

