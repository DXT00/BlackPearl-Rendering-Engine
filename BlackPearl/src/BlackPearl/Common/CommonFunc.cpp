#include "pch.h"
#include "CommonFunc.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
namespace BlackPearl {
	void CommonFunc::ShowGBuffer(unsigned int row, unsigned int col, Object* quad,std::shared_ptr<GBuffer> gBuffer, std::vector<std::shared_ptr<Texture>> textures)
	{
		gBuffer->UnBind();
		int width  = Configuration::WindowWidth / col;
		int height = Configuration::WindowHeight / row;
		std::shared_ptr<Shader> shader(DBG_NEW Shader("assets/shaders/raytracing/ScreenQuad.glsl"));
		BasicRenderer* basicRenderer = new BasicRenderer();
		
		//gBuffer->Bind();
		for (int j = 0; j < col;j++)
		{
			for (int i = 0; i < row; i++)
			{
				int idx = j * row + i;
				if (idx >= textures.size())break;
				int x = j * width, y = i * height;
				glViewport(x, y, width, height);

				shader->Bind();
				shader->SetUniform1i("u_FinalScreenTexture", idx);
				glActiveTexture(GL_TEXTURE0+ idx);
				textures[idx]->Bind();

				basicRenderer->DrawObject(quad,shader);
			}
		}
		//gBuffer->UnBind();
		//delete basicRenderer;
	}
}

