#pragma once

#include "Nibbler/Core/Core.hpp"
#include "Nibbler/Events/Event.hpp"

namespace Nibbler
{

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(uint32_t width, uint32_t height)
		: _width(width), _height(height) {}

	uint32_t	GetWidth() { return _width; }
	uint32_t	GetHeight() { return _height; }

	std::string	ToString() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << _width << ", " << _height;
		return ss.str();
	}

	EVENT_CLASS_TYPE(NIB_EventType::WindowResize);
	EVENT_CLASS_CATEGORY(EventCategoryApplication);

private:
	uint32_t	_width, _height;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() = default;

	EVENT_CLASS_TYPE(NIB_EventType::WindowClose);
	EVENT_CLASS_CATEGORY(EventCategoryApplication);
};

class AppTickEvent : public Event
{
public:
	AppTickEvent() = default;

	EVENT_CLASS_TYPE(NIB_EventType::AppTick);
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppUpdateEvent : public Event
{
public:
	AppUpdateEvent() = default;

	EVENT_CLASS_TYPE(NIB_EventType::AppUpdate);
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppRenderEvent : public Event
{
public:
	AppRenderEvent() = default;

	EVENT_CLASS_TYPE(NIB_EventType::AppRender);
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

} // Nibbler
