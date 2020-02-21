#include "pch.h"
#include "GBufferRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"




namespace BlackPearl {
	GBufferRenderer::GBufferRenderer()
	{
		m_GBffer.reset(DBG_NEW GBuffer(m_TextureWidth, m_TexxtureHeight));
		m_GBufferShader.reset(DBG_NEW Shader("assets/shaders/gBuffer/gBuffer.glsl"));
		m_LightingShader.reset(DBG_NEW Shader("assets/shaders/gBuffer/gBufferLighting.glsl"));
	}

	void GBufferRenderer::Init(Object* ScreenQuad)
	{
		m_ScreenQuad = ScreenQuad;
		m_IsInitialized = true;
	}

	void GBufferRenderer::Render(std::vector<Object*> objects,Object * gBufferDebugQuad)
	{
		GE_ASSERT(m_IsInitialized,"GBufferRenderer have not been initialized! ")
		
		// 1. Geometry Pass: render scene's geometry/color data into gbuffer
		m_GBffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		m_GBufferShader->Bind();
		DrawObjects(objects,m_GBufferShader);

		m_GBffer->UnBind();

	//	DrawGBuffer(gBufferDebugQuad);

		// 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_LightingShader->Bind();
		m_LightingShader->SetUniform1i("gPosition", 0);
		m_LightingShader->SetUniform1i("gNormal", 1);
		m_LightingShader->SetUniform1i("gAlbedoSpec", 2);
		m_LightingShader->SetUniform1i("gSpecular", 3);

		glActiveTexture(GL_TEXTURE0);
		m_GBffer->GetPositionTexture()->Bind();
		glActiveTexture(GL_TEXTURE1);
		m_GBffer->GetNormalTexture()->Bind();
		glActiveTexture(GL_TEXTURE2);
		m_GBffer->GetAlbedoTexture()->Bind();
		glActiveTexture(GL_TEXTURE3);
		m_GBffer->GetSpecularTexture()->Bind();

		DrawObject(m_ScreenQuad, m_LightingShader);







	}

	void GBufferRenderer::DrawGBuffer(Object* gBufferDebugQuad)
	{

		glViewport(0, 270, 480, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBffer->GetPositionTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(0, 0, 480, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBffer->GetNormalTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(480, 0, 480, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBffer->GetAlbedoTexture());
		DrawObject(gBufferDebugQuad);


		glViewport(480, 270, 480, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBffer->GetSpecularTexture());
		DrawObject(gBufferDebugQuad);
	}

}
