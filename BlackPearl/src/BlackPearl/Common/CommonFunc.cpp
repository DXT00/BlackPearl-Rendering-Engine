#include "pch.h"
#include "CommonFunc.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
namespace BlackPearl {
	void CommonFunc::ShowGBuffer(unsigned int row, unsigned int col, Object* quad,std::shared_ptr<GBuffer> gBuffer, std::vector<TextureHandle> textures)
	{
		gBuffer->UnBind();
		ShowTextures(row, col, quad, textures);

		//gBuffer->UnBind();
		//delete basicRenderer;
	}
	void CommonFunc::ShowFrameBuffer(unsigned int row, unsigned int col, Object* quad, std::shared_ptr<FrameBuffer> frameBuffer, std::vector<TextureHandle> textures)
	{
		//frameBuffer->UnBind();
		ShowTextures(row, col, quad, textures);
	}

	void CommonFunc::ShowFrameBuffer(glm::vec4 viewPort, Object* quad, std::shared_ptr<FrameBuffer> frameBuffer, TextureHandle texture, bool isMipmap, int lod)
	{
		frameBuffer->UnBind();
		ShowTexture(viewPort, quad, texture, isMipmap, lod);
	}

	void CommonFunc::ShowTextures(unsigned int row, unsigned int col, Object* quad, std::vector<TextureHandle> textures)
	{
		int width = Configuration::WindowWidth / col;
		int height = Configuration::WindowHeight / row;
		std::shared_ptr<Shader> shader(DBG_NEW Shader("assets/shaders/raytracing/ScreenQuad.glsl"));
		BasicRenderer* basicRenderer = new BasicRenderer();

		//gBuffer->Bind();
		for (int j = 0; j < col; j++)
		{
			for (int i = 0; i < row; i++)
			{
				int idx = j * row + i;
				if (idx >= textures.size())break;
				int x = j * width, y = i * height;
				glViewport(x, y, width, height);

				shader->Bind();
				shader->SetUniform1f("u_Num", 1.0);
				shader->SetUniform1i("u_FinalScreenTexture", idx);
				if (textures[idx]->GetType() == TextureType::DepthMap) {
					shader->SetUniform1i("u_isDepth", 1);
				}
				glActiveTexture(GL_TEXTURE0 + idx);
				textures[idx]->Bind();

				basicRenderer->DrawObject(quad, shader);
			}
		}
	}
	void CommonFunc::ShowTexture(glm::vec4 viewPort, Object* quad, TextureHandle texture, bool isMipmap, int lod)
	{
		
		std::shared_ptr<Shader> shader(DBG_NEW Shader("assets/shaders/raytracing/ScreenQuad.glsl"));
		BasicRenderer* basicRenderer = new BasicRenderer();
	
		glViewport(viewPort.x, viewPort.y, viewPort.z, viewPort.w);

		shader->Bind();
		shader->SetUniform1f("u_Num", 1.0);
		shader->SetUniform1i("u_FinalScreenTexture", lod);
		if (texture->GetType() == Texture::DepthMap) {
			shader->SetUniform1i("u_isDepth", 1);
		}
		if (isMipmap) {
			shader->SetUniform1i("u_isMipmap", 1);
			shader->SetUniform1i("u_Lod", lod);
		}
		else {
			shader->SetUniform1i("u_isMipmap", 0);
		}
		glActiveTexture(GL_TEXTURE0 + lod);
		texture->Bind();

		basicRenderer->DrawObject(quad, shader);
			
		

	}
}

