### LightProbes --Render SpecularPrefilterMap

#### 1.更新SpecularPrefilterMap，每次按下U键更新所有lightProbes的SpecularMap（不需要每帧都更新，否则会卡死。。）


#### 2.更新SpecularPrefilterMap,采用mipmap存储

IBLProbesRenderer class:

```
		void IBLProbesRenderer::RenderSpecularPrefilterMap(const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	{
		glm::vec3 center = probe->GetPosition();
		probe->UpdateCamera();
		auto camera = probe->GetCamera();
		auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

		auto projection = cameraComponent->GetProjectionMatrix();
		std::vector<glm::mat4> ProbeView = {
			glm::lookAt(center, center - camera->Front(),-camera->Up()),
			glm::lookAt(center, center + camera->Front(),-camera->Up()),
			glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
			glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()
			glm::lookAt(center, center - camera->Right(), -camera->Up()),
			glm::lookAt(center, center + camera->Right(), -camera->Up())

		};
		std::vector<glm::mat4> ProbeProjectionViews = {
			projection * ProbeView[0],
			projection * ProbeView[1],
			projection * ProbeView[2],
			projection * ProbeView[3],
			projection * ProbeView[4],
			projection * ProbeView[5]
		};
		auto specularIrradianceMap = probe->GetSpecularPrefilterCubeMap();
		m_SpecularPrefilterShader->Bind();
		m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		probe->GetHdrEnvironmentCubeMap()->Bind();

		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());

		
		frameBuffer->AttachCubeMapColorTexture(0, specularIrradianceMap);
		frameBuffer->AttachRenderBuffer(specularIrradianceMap->GetWidth(), specularIrradianceMap->GetHeight());
		frameBuffer->Bind();
		unsigned int maxMipMapLevels = 5;
		for (unsigned int mip = 0; mip < maxMipMapLevels; mip++)
		{
			//resize framebuffer according to mipmap-level size;
			unsigned int mipWidth = specularIrradianceMap->GetWidth() * std::pow(0.5, mip);
			unsigned int mipHeight = specularIrradianceMap->GetHeight() * std::pow(0.5, mip);
			frameBuffer->BindRenderBuffer();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipMapLevels - 1);
			m_SpecularPrefilterShader->SetUniform1f("u_roughness", roughness);


			for (unsigned int i = 0; i < 6; i++)
			{
				
				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),cameraComponent->Front(),lightSources });
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specularIrradianceMap->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				probe->GetObj()->GetComponent<MeshRenderer>()->SetShaders(m_SpecularPrefilterShader);

				DrawObject(probe->GetObj(), m_SpecularPrefilterShader, scene);
				delete scene;
				scene = nullptr;

			}
		}


		frameBuffer->UnBind();
		frameBuffer->CleanUp();

	}

```

#### 3.修改LightProbe结构-->每个lightProbe有一个Camera来采集6个面图像

注意Camera的初始化 ：Yaw=-90,Pitch = 0

```
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetFov(90.0f);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetWidth(512);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetHeight(512);


```
注意：Camera的Transform和LightProbe Object的Transsform一致，每次采集
ProjectionView Matrix时要 UpdateCamera,使得Camera的位置、旋转和lightProbe
Object同步！

```

glm::vec3 center = probe->GetPosition();
		probe->UpdateCamera();
		auto camera = probe->GetCamera();
		auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

		auto projection = cameraComponent->GetProjectionMatrix();
		std::vector<glm::mat4> ProbeView = {
			glm::lookAt(center, center - camera->Front(),-camera->Up()),
			glm::lookAt(center, center + camera->Front(),-camera->Up()),
			glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
			glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()
			glm::lookAt(center, center - camera->Right(), -camera->Up()),
			glm::lookAt(center, center + camera->Right(), -camera->Up())

		};
		std::vector<glm::mat4> ProbeProjectionViews = {
			projection * ProbeView[0],
			projection * ProbeView[1],
			projection * ProbeView[2],
			projection * ProbeView[3],
			projection * ProbeView[4],
			projection * ProbeView[5]
		};

```

#### 4.增加MainCamera class，用于统一Camera Object的 PerspectiveCamera Component和TransForm Component


```
class MainCamera
	{
	public:
		MainCamera(Object* cameraObj) {
			m_CameraObj = cameraObj;
		}
		glm::vec3 GetPosition() const;
		float Yaw() const;
		float Pitch() const;
		glm::vec3 Front() const;
		glm::vec3 Up() const;
		glm::vec3 Right() const;

		void SetPosition(glm::vec3 pos);
		void SetRotation(glm::vec3 rotation);
		Object* GetObj() const { return m_CameraObj; }
	private:
		Object* m_CameraObj = nullptr;


	};

```

#### 5.修改 SpecularPrefilterMap的vertex shader prefilterMap.glsl :添加 u_Model。

注意：在IBLRenderer 中prefilterMap.glsl 是用来渲染天空盒子的
而在 IBLProbeRenderer中prefilterMap.glsl 是用来渲染 lightProbes的，所以必须加上u_Model！

```
#type vertex
#version 430 core

layout (location = 0) in vec3 aPos;


uniform mat4 u_ProjectionView;
uniform mat4 u_Model;

uniform mat4 u_CubeMapProjectionView;
out vec3 WorldPos;

void main(){

	WorldPos = aPos;
	//这里注意：渲染Skybox时不需要u_Model,LightProbes时需要
	gl_Position = u_ProjectionView*u_Model*vec4(WorldPos,1.0);
	//gl_Position = u_ProjectionView*vec4(WorldPos,1.0);

}

```



![UpdateSpecularIrradianceMap](/results/lightProbesSpecularMapUpdate1.png)
![UpdateSpecularIrradianceMap](/results/lightProbesSpecularMapUpdate2.png)
![UpdateSpecularIrradianceMap](/results/lightProbesSpecularMapUpdate3.png)



