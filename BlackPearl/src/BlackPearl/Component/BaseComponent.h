#pragma once

namespace BlackPearl {
	
	/**
 * Base component class, only used for insertion into collections.
 *
 * Family is used for registration.
 */
	class BaseComponent {
	public:
		typedef size_t Family;
		enum Type {
			Transform,
			MeshRenderer,
			MeshFilter,
			Light,
			Camera,
			LightProbe,
			Hitable,
			BasicInfo

		};
		BaseComponent(Type type)
			:m_Type(type){}
		// NOTE: Component memory is *always* managed by the EntityManager.
		// Use Entity::destroy() instead.
		//void operator delete(void *p) { fail(); }
		//void operator delete[](void *p) { fail(); }
		virtual ~BaseComponent(){}
		static Family s_FamliyCounter;
		virtual void ToString(){}
		Type GetType() { return m_Type; }
	protected:
		Type m_Type;
		static void fail() {
#if defined(_HAS_EXCEPTIONS) || defined(__EXCEPTIONS)
			throw std::bad_alloc();
#else
			std::abort();
#endif
		}


	};
}

