#pragma once
#include "Event.h"
#include <sstream>
class MouseMovedEvent :public Event{
public:
	MouseMovedEvent(double x, double y) :
		m_Xpos(x), m_Ypos(y){}

	virtual inline EventType GetEventType()const override { return MouseMoved; };
	virtual inline int GetEventCategory() const override{ return EventCategoryInput |EventCategoryMouse;}
	virtual const char* GetName() const override { return "MouseMovedEvent"; }
	virtual inline std::string ToString() const override{
		std::stringstream ss;
		ss << "MouseMovedEvent" << GetMouseX() << "," << GetMouseY() ;
		return ss.str();
	}
	inline double GetMouseX() const { return m_Xpos; }
	inline double GetMouseY() const { return m_Ypos; }

private:
	double m_Xpos;
	double m_Ypos;
};


