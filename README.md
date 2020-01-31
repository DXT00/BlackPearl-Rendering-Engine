### LightProbes --Render EnvironmentMap

#### 1.添加3个LightProbes

LightProbes class:

```
#pragma once
#include "glm/glm.hpp"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {

	/*每个probe 都由一个 CubeObj 代表*/
	class LightProbe
	{
	public:
		LightProbe(Object* cubeObj);
		virtual ~LightProbe() {

			delete m_CubeObj;
			m_CubeObj = nullptr;
		}
		
		/* probe's view matrix */
		glm::mat4	GetViewMatrix()const { return m_ViewMatrix; }

		/* Textures */
		std::shared_ptr<CubeMapTexture> GetHdrEnvironmentCubeMap()const    { return m_HdrEnvironmentCubeMap; }
		std::shared_ptr<CubeMapTexture> GetSpecularPrefilterCubeMap()const { return m_SpecularPrefilterCubeMap; }
		std::shared_ptr<CubeMapTexture> GetDiffuseIrradianceCubeMap()const { return m_DiffuseIrradianceCubeMap; }
		std::shared_ptr<Texture> GetSpecularBrdfLutMap()const       { return m_SpecularBrdfLutMap; }
		/* resolution */
		unsigned int GetDiffuseCubeMapResolution() const { return m_DiffuseCubeMapResolution; }
		unsigned int GetSpecularCubeMapResolution() const { return m_SpecularCubeMapResolution; }

		/* */
		unsigned int GetMaxMipMapLevel() const{ return m_MaxMipmapLevel; }
		glm::vec3	 GetPosition()const { return m_Center; }

		/*set*/
		void SetPosition(glm::vec3 pos);
		/*cubeObj*/
		Object* GetCubeObj()const { return m_CubeObj; }

	private:

		/* probe's view matrix */
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		/* Textures */
		std::shared_ptr<CubeMapTexture> m_HdrEnvironmentCubeMap = nullptr;
		std::shared_ptr<CubeMapTexture> m_SpecularPrefilterCubeMap = nullptr;
		std::shared_ptr<CubeMapTexture> m_DiffuseIrradianceCubeMap = nullptr;
		std::shared_ptr<Texture>		m_SpecularBrdfLutMap = nullptr;

		unsigned int m_MaxMipmapLevel = 5;

		unsigned int					m_SampleCounts = 1024;
		unsigned int					m_EnvironmentCubeMapResolution = 512;
		unsigned int					m_DiffuseCubeMapResolution = 32;
		unsigned int					m_SpecularCubeMapResolution = 128;

		glm::vec3 m_Center;
		
		Object* m_CubeObj;
	};


}



```

#### 2.添加IBLProbesRenderer

IBLProbesRenderer class:

```
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


```

#### 3.为每个lightProbes渲染环境贴图,并显示在lightProbe的CubeObject上


```
void IBLProbesRenderer::Render(Object* ProbesCamera, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes)
	{
		GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");
		RenderSpecularBRDFLUTMap();

		auto cameraComponent = ProbesCamera->GetComponent<PerspectiveCamera>();
		float zNear = cameraComponent->GetZnear();
		float zFar = cameraComponent->GetZfar();
		float fov = cameraComponent->GetFov();
		m_ProbeProjection = glm::perspective(glm::radians(fov), 1.0f, zNear, zFar);


		for (auto it = probes.begin(); it != probes.end(); it++)
		{
			LightProbe* probe = *it;
			glm::vec3 center = probe->GetPosition();
			m_ProbeView = {
			   glm::lookAt(center, glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			   glm::lookAt(center, glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

			};
			m_ProbeProjectionViews = {
			   m_ProbeProjection * m_ProbeView[0],
			   m_ProbeProjection * m_ProbeView[1],
			   m_ProbeProjection * m_ProbeView[2],
			   m_ProbeProjection * m_ProbeView[3],
			   m_ProbeProjection * m_ProbeView[4],
			   m_ProbeProjection * m_ProbeView[5]
			};
			RenderEnvironmerntCubeMaps(ProbesCamera, lightSources, objects, probe);
			//RenderDiffuseIrradianceMap(ProbesCamera, lightSources, objects, probe);
		//	RenderSpecularPrefilterMap(ProbesCamera, lightSources, objects, probe);
		//	probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetSpecularPrefilterCubeMap());
			//probe->GetHdrEnvironmentCubeMap()
			probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetHdrEnvironmentCubeMap());
			probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetShaders(m_LightProbeShader);
			DrawObject(probe->GetCubeObj(), m_LightProbeShader);

		}





	}
```

#### 存在bug-->天空盒子反了。。

#### 存在bug-->lightProbes每一帧都更新，占用内存太大。。。

![LightProbes](/results/lightProbes.jpg)



