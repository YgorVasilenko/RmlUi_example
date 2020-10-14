#pragma once
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core.h>

class myEventListener : public Rml::EventListener {

	virtual void ProcessEvent(Rml::Event& event);

};