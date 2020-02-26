#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
namespace BlackPearl {
	class AnimatedModelRenderer:public BasicRenderer
	{
	public:
		AnimatedModelRenderer() {

			m_AnimatedShader.reset(DBG_NEW Shader("assets/shaders/animatedModel/animatedModel.glsl"));
		}
		void Render(Object* animatedModel,float timeInSecond);

		void SetShader(std::shared_ptr<Shader> shader) {
			m_AnimatedShader  = shader;
		}
		std::shared_ptr<Shader> GetShader() const{
			return m_AnimatedShader;
		}
	private:
		std::shared_ptr<Shader> m_AnimatedShader;



	};
}


