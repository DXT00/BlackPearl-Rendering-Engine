#pragma once
#include"BlackPearl/Layer.h"
#include "BlackPearl/Timestep/Timestep.h"
namespace BlackPearl {

	class ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer(const std::string& name,  EntityManager *entityManager) 
			:Layer(name,entityManager) {}
		virtual ~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImguiRender() override;


		void Begin();
		void End();


	};

}