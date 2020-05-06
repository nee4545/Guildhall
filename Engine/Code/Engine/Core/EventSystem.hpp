#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include <vector>
#include <map >

typedef NamedProperties EventArgs;
typedef bool(*EventCallBackFunction)(EventArgs& args);

struct EventSubscription
{
	std::string m_eventName;
	EventCallBackFunction m_eventToListen = nullptr;
};

class EventSystem
{
private:
	std::vector<EventSubscription*> m_subscriptions; 
public:
	void StartUp();
	void ShutDown();
	void SubscribeToEvent( const std::string& eventName, EventCallBackFunction eventToListen );
	void FireEvent(const std::string& eventName,EventArgs& args);
	void UnSubscribeToEvent( const std::string& eventName, EventCallBackFunction eventToUnscribe);
	  
};



