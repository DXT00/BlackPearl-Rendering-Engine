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

		// NOTE: Component memory is *always* managed by the EntityManager.
		// Use Entity::destroy() instead.
		//void operator delete(void *p) { fail(); }
		//void operator delete[](void *p) { fail(); }
		virtual ~BaseComponent(){}
		static Family s_FamliyCounter;
		virtual void ToString(){}
	protected:
		static void fail() {
#if defined(_HAS_EXCEPTIONS) || defined(__EXCEPTIONS)
			throw std::bad_alloc();
#else
			std::abort();
#endif
		}


	};
}

