#pragma once
#include <string>
#include "BlackPearl/Config.h"
#include "BlackPearl/Core.h"
#include <unordered_map>
namespace BlackPearl {
	//template<typename C>
	//class ComponentHandle;
	class Entity
	{
	public:
		//	static const unsigned int s_MaxComponents;//һ��Entity���32��Component!//TODO::��Щ�������д��һ���ļ��

		struct Id {
			Id() :id(0) {}
			explicit Id(std::uint64_t id) :id(id) {}
			Id(std::uint32_t index, std::uint32_t version) :id(uint64_t(index) | uint64_t(version) << 32UL) {}

			std::uint32_t index() { return id & 0xffffffffUL; }
			std::uint32_t version() { return id >> 32UL; }
			std::uint64_t id;
			bool operator == (const Id Other)const {
				return id == Other.id;
			}

			Id &operator = (const Id &Other) {
				
				this->id = Other.id;
				return *this;
			}

		};
		static const Id s_INVALID;

		Entity(Id id)
			: m_Id(id) {
		}

		bool operator == (const Entity &other)const {
			return m_Id == other.GetId();
		}

		//Entity Instantiate(Entity original, glm::vec3 position) { s_InstanceID++; };
		virtual ~Entity() { Destroy(); };

		Id GetId() const { return m_Id; }
		//EntityManager* GetEntityManager() const { return m_EntityManager; }
	public:
		bool Vaild();
		virtual void Destroy() {
			m_Id = Entity::s_INVALID;

		};


	private:
		Id m_Id = Entity::s_INVALID;
		//EntityManager* m_EntityManager = nullptr;



	};




	//////////////////////////////EntityManager///////////////////////////////////

	class EntityManager
	{
	public:
		EntityManager();
		virtual ~EntityManager() {
			for (auto& entity : m_EntityList)
				GE_SAVE_DELETE(entity);
		};
	public:
		inline bool Valid(BlackPearl::Entity::Id id) {
			return id.index() < m_EntityVersion.size() && m_EntityVersion[id.index()] == id.version();
		}
		inline void AssertValid(Entity::Id id) const {
			GE_ASSERT((id.index() < m_EntityList.size()), "Entity::Id ID outside entity vector range");
			GE_ASSERT(m_EntityVersion[id.index()] == id.version(), "Attempt to access Entity via a stale Entity::Id");
		}


	public:

		Entity *CreateEntity();

		void DestroyEntity(Entity::Id entity);

		inline size_t GetEntitiesNum() const {
			return m_EntityList.size() - m_FreeList.size();
		}
		inline size_t GetEntitiesCapacity() const {
			return m_EntityList.size();
		}


		Entity GetEntity(Entity::Id id);

		void ResizeEntityStorage(std::uint32_t index);

		void ResizeConponentStorage(std::uint32_t index);


	private:
		std::vector<Entity*> m_EntityList;  // Entity::id.index --->�� Entity::id��ӳ�䣻
		std::vector<unsigned int> m_FreeList;//��¼��Ч��Entity
		std::vector<std::uint32_t> m_EntityVersion;//��¼ͬһ��Entity�İ汾�ţ��൱��prefab�Ķ��ʵ��),ÿ��destroyһ��Entity,Vertion�ż�һ��
		std::uint32_t m_IndexCounter = 0;
	};

}