#pragma once
#include "Event.h"
#include "KeyCodes.h"

namespace BlackPearl {
	class KeyPressedEvent :public Event {
	public:
		KeyPressedEvent(uint32_t key) :
			m_KeyCode(key) {
		}

		virtual inline EventType GetEventType()const override { return KeyPressed; };
		virtual inline int GetEventCategory() const override { return EventCategoryInput | EventCategoryKeyBoard; }
		virtual const char* GetName() const override { return "KeyPressedEvent"; }
		virtual inline std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent" << GetKeyCode();
			return ss.str();
		}
		inline uint32_t GetKeyCode() const { return m_KeyCode; }

	private:
		uint32_t m_KeyCode;
	};


	class KeyReleasedEvent :public Event {
	public:
		KeyReleasedEvent(uint32_t key) :
			m_KeyCode(key) {
		}

		virtual inline EventType GetEventType()const override { return KeyReleased; };
		virtual inline int GetEventCategory() const override { return EventCategoryInput | EventCategoryKeyBoard; }
		virtual const char* GetName() const override { return "KeyPressedEvent"; }
		virtual inline std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent" << GetKeyCode();
			return ss.str();
		}
		inline uint32_t GetKeyCode() const { return m_KeyCode; }

	private:
		uint32_t m_KeyCode;
	};

}