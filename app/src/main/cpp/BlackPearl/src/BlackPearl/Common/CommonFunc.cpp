#include "pch.h"
#include "Common/CommonFunc.h"
#include "Config.h"
#include "Renderer/MasterRenderer/BasicRenderer.h"
#include "Component/MeshRendererComponent/MeshRenderer.h"

namespace BlackPearl {
	//TODO:: Gbuffer to all api
#ifdef GE_API_OPENGL
	void CommonFunc::ShowGBuffer(unsigned int row, unsigned int col, Object* quad,std::shared_ptr<GBuffer> gBuffer, std::vector<TextureHandle> textures)
	{
		gBuffer->UnBind();
		ShowTextures(row, col, quad, textures);

		//gBuffer->UnBind();
		//delete basicRenderer;
	}
#endif
	void CommonFunc::ShowFrameBuffer(unsigned int row, unsigned int col, Object* quad, IBuffer* frameBuffer, std::vector<TextureHandle> textures)
	{
		//frameBuffer->UnBind();
		ShowTextures(row, col, quad, textures);
	}

	void CommonFunc::ShowFrameBuffer(glm::vec4 viewPort, Object* quad, IBuffer* frameBuffer, TextureHandle texture, bool isMipmap, int lod)
	{
		/*frameBuffer->UnBind();
		ShowTexture(viewPort, quad, texture, isMipmap, lod);*/
	}

	void CommonFunc::ShowTextures(unsigned int row, unsigned int col, Object* quad, std::vector<TextureHandle> textures)
	{
		//int width = Configuration::WindowWidth / col;
		//int height = Configuration::WindowHeight / row;
		//std::shared_ptr<Shader> shader(DBG_NEW Shader("assets/shaders/glsl/raytracing/ScreenQuad.glsl"));
		//BasicRenderer* basicRenderer = new BasicRenderer();

		////gBuffer->Bind();
		//for (int j = 0; j < col; j++)
		//{
		//	for (int i = 0; i < row; i++)
		//	{
		//		int idx = j * row + i;
		//		if (idx >= textures.size())break;
		//		int x = j * width, y = i * height;
		//		glViewport(x, y, width, height);

		//		shader->Bind();
		//		shader->SetUniform1f("u_Num", 1.0);
		//		shader->SetUniform1i("u_FinalScreenTexture", idx);
		//		if (textures[idx]->GetType() == TextureType::DepthMap) {
		//			shader->SetUniform1i("u_isDepth", 1);
		//		}
		//		glActiveTexture(GL_TEXTURE0 + idx);
		//		textures[idx]->Bind();

		//		basicRenderer->DrawObject(quad, shader);
		//	}
		//}
	}
	void CommonFunc::ShowTexture(glm::vec4 viewPort, Object* quad, TextureHandle texture, bool isMipmap, int lod)
	{
		
		/*std::shared_ptr<Shader> shader(DBG_NEW Shader("assets/shaders/glsl/raytracing/ScreenQuad.glsl"));
		BasicRenderer* basicRenderer = new BasicRenderer();
	
		glViewport(viewPort.x, viewPort.y, viewPort.z, viewPort.w);

		shader->Bind();
		shader->SetUniform1f("u_Num", 1.0);
		shader->SetUniform1i("u_FinalScreenTexture", lod);
		if (texture->GetType() == TextureType::DepthMap) {
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
			*/
		

	}


    std::pair<std::string, std::string> CommonFunc::SplitString(
            const std::string& str,
            const std::string& delimiter
    ) {
        size_t pos = str.find(delimiter);
        if (pos == std::string::npos) {
            return {str, ""};
        }
        return {
                str.substr(0, pos),
                str.substr(pos + delimiter.length())
        };
    }

    std::vector<std::string> CommonFunc::SplitString(const std::string& str, char delimiter)
    {
        std::vector<std::string> subStrings;
        std::size_t start = 0;
        for(std::size_t pos = 0; pos < str.size(); ++pos ) {
            if( str[pos] == delimiter ) {
                if( pos - start > 1 )
                    subStrings.push_back( str.substr( start, pos-start ) );
                start = pos+1;
            }
        }
        if( start < str.size() )
            subStrings.push_back( str.substr( start, str.size()-start ) );
        return subStrings;
    }
}

