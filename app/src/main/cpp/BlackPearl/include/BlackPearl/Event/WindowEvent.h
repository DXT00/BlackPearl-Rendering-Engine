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


    class WindowResizeEvent :public Event {
    public:
        WindowResizeEvent(int width, int height)
        {
            m_Width = width;
            m_Height = height;
        }

        virtual  EventType GetEventType()const override { return WIndowResize; };
        virtual  int GetEventCategory() const override { return EventCategoryInput | EventCategoryWindow; }
        virtual const char* GetName() const override { return "WindowResizeEvent"; }
        virtual  std::string ToString() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent";
            return ss.str();
        }
        //inline double GetKeyCode() const { return m_KeyCode; }
        inline uint32_t GetWidth() const {
            return m_Width;
        }
        inline uint32_t GetHeight() const {
            return m_Height;
        }
    private:
        uint32_t m_Width;
        uint32_t m_Height;
        //uint32_t m_KeyCode;
    };

}