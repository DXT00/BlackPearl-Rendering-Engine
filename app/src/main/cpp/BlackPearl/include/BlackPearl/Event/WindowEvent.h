#pragma once
#include "Event.h"
#include "KeyCodes.h"

namespace BlackPearl {
	class WindowCloseEvent :public Event {
	public:
		WindowCloseEvent() 
		{
		}

		virtual inline EventType GetEventType()const override { return WindowClose; };
		virtual inline int GetEventCategory() const override { return EventCategoryInput | EventCategoryWindow; }
		virtual const char* GetName() const override { return "WindowCloseEvent"; }
		virtual inline std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowCloseEvent" ;
			return ss.str();
		}
		//inline double GetKeyCode() const { return m_KeyCode; }

	private:
		//uint32_t m_KeyCode;
	};
}