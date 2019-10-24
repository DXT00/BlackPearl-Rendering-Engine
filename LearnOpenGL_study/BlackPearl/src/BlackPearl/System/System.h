#pragma once
#include <unordered_map>
#include <memory>
#include "BlackPearl/System/BaseSystem.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Entity/Entity.h"
namespace BlackPearl {

	class System:public BaseSystem
	{
	public:
		System();
		~System();

		//client µ÷ÓÃ
		virtual void Configure() {}; 
		virtual void Update(Timestep ts) {};

		static BaseSystem::Family Famliy() {
			/*static BaseSystem::Family famliy = BaseSystem::s_FamliyCounter++;
			return famliy;*/
		}


	};



	/////////////////////////SystemManager//////////////////////////////


	class SystemManager {
	public:
		SystemManager(EntityManager &entityManager)
			:m_EntityManager(entityManager),m_Initialized(false){}

		template<typename S>
		void Add(std::shared_ptr<S> system) {

			m_Systems.insert(std::make_pair(S::Famliy(), system));
		}

		void Add(BaseSystem::Family family, std::shared_ptr<System> system) {

			m_Systems.insert(std::make_pair(family, system));
		}

		template<typename S,typename ...Args>
		void Add(Args && ...args) {
			std::shared_ptr<S> system = std::make_shared<S>(std::forward(args));
			Add(system);
		}

		void Configure() {

			for (auto &pair : m_Systems) {
				pair.second->Configure();
			}
			
			m_Initialized = true;

		}
		template<typename S>
		std::shared_ptr<S> GetSystem() {
			BaseSystem::Family famliy = S::Famliy();
			auto it = m_Systems.find(famliy);
			GE_ASSERT(it!=m_Systems.end(),"GetSystem failed!")
			if (it != m_Systems.end()) {
				return std::dynamic_pointer_cast<S>(it->second);
			}
		
		}

		template<typename S>
		void Update(Timestep ts) {
			GE_ASSERT(m_Initialized, "SystemManager::Configure not called!");
			GetSystem<S>()->Update();
		}
		void UpdateAll(Timestep ts) {
			GE_ASSERT(m_Initialized,"SystemManager::Configure not called!")
			for (auto &pair :m_Systems) {
				pair.second->Update(ts);
			}
		}
	
	private:
		std::unordered_map<BaseSystem::Family, std::shared_ptr<BaseSystem>> m_Systems;
		bool m_Initialized;
		EntityManager& m_EntityManager;
	};


}
