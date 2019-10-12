#pragma once
#include"Timestep/Timestep.h"
#include<string>
namespace BlackPearl {

	class Layer
	{
	public:
		Layer(const std::string& name)
			:m_DebugName(name) {};
		virtual ~Layer() = default;
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImguiRender() {}

		inline std::string GetString() { return m_DebugName; }


	private:
		std::string m_DebugName;
	};

}