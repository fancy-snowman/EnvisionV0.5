#include "envision\envpch.h"
#include "envision\core\EventBusObject.h"
#include "envision\core\Application.h"

env::EventBusObject::EventBusObject(Application& application) :
	m_application(application)
{
	//
}

env::EventBusObject::~EventBusObject()
{
	//
}

env::EventBusObject::EventBusObject(const EventBusObject& other) :
	m_application(other.m_application)
{
	//
}

void env::EventBusObject::PublishEvent(Event& event)
{
	m_application.PublishEvent(event);
}
