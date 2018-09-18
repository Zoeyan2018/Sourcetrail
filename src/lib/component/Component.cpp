#include "Component.h"

#include "View.h"
#include "Controller.h"

Component::Component(std::shared_ptr<View> view, std::shared_ptr<Controller> controller)
	: m_controller(controller)
	, m_view(view)
{
	if (m_controller)
	{
		m_controller->setComponent(this);
	}

	if (m_view)
	{
		m_view->setComponent(this);
	}
}

Component::~Component()
{
	if (m_controller)
	{
		m_controller->setComponent(nullptr);
	}

	if (m_view)
	{
		m_view->setComponent(nullptr);
	}
}

Controller* Component::getControllerPtr() const
{
	return m_controller.get();
}

View* Component::getViewPtr() const
{
	return m_view.get();
}
