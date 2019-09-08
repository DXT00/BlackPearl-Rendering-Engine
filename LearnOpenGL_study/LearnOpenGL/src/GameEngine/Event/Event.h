#pragma once
#include "pch.h"
#include "GameEngine/Core.h"
enum EventType {
	KeyPressed, KeyReleased, KeyTyped,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};
enum EventCategory {
	None = 0,
	EventCategoryApplication  = BIT(0),
	EventCategoryInput        = BIT(1),
	EventCategoryKeyBoard     = BIT(2),
	EventCategoryMouse        = BIT(3),
	EventCategoryMouseButton  = BIT(4)
};
class Event
{
public:
	virtual ~Event() = default;
	virtual inline EventType GetEventType()const = 0;
	virtual const char* GetName() const = 0;
	virtual inline int GetEventCategory() const = 0;
	virtual inline std::string ToString() const = 0;
};

class EventDispacher {

public:
	EventDispacher(Event& event)
		:m_Event(event){}

	template<typename T>
	bool Dispatch(std::function<bool(T&)> func) {

		return true;

	}
private:
	Event& m_Event;
};

