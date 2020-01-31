#pragma once
#include "BlackPearl/Renderer/MasterRenderer/IBLRenderer.h"
#include "BlackPearl/LightProbes/LightProbes.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {
	
	class IBLProbesRenderer:public IBLRenderer
	{
	public:
		IBLProbesRenderer();
		virtual ~IBLProbesRenderer();
		void Render(Object* ProbesCamera, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes);
		void Init(Object* ProbesCamera, Object* brdfLUTQuadObj, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes);
		std::shared_ptr<Texture> GetSpecularBrdfLUTTexture() const { return m_SpecularBrdfLUTTexture; }

	private:
		/*render environment CubeMap of each probe */
		void RenderEnvironmerntCubeMaps(Object* ProbesCamera, const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probes);
		void RenderDiffuseIrradianceMap(Object* ProbesCamera, const LightSources& lightSources, std::vector<Object*> objects, LightProbe *probe);
		void RenderSpecularPrefilterMap(Object* ProbesCamera, const LightSources& lightSources, std::vector<Object*> objects, LightProbe *probe);

		void RenderSpecularBRDFLUTMap();

	

	private:
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
	};

}

