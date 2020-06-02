#include "Engine/Core/EventSystem.hpp"

void EventSystem::StartUp()
{

}

void EventSystem::ShutDown()
{

}

void EventSystem::SubscribeToEvent( const std::string& eventName , EventCallBackFunction eventToListen )
{
	EventSubscription* newSubscription = new EventSubscription();
	newSubscription->m_eventName = eventName;
	newSubscription->m_eventToListen = eventToListen;
	m_subscriptions.push_back( newSubscription );
}


void EventSystem::FireEvent( const std::string& eventName , EventArgs& args )
{
	for ( int eventIndex = 0; eventIndex < ( int ) m_subscriptions.size(); eventIndex++ )
	{
		if ( m_subscriptions[ eventIndex ] != nullptr )
		{
			EventSubscription* subscription = m_subscriptions[ eventIndex ];
			if ( subscription->m_eventName == eventName )
			{
				subscription->m_eventToListen( args );
			}
		}
	}

	for ( int i = 0; i < ( int ) m_methodSubscriptions.size(); i++ )
	{
		if ( m_methodSubscriptions[ i ] != nullptr )
		{
			if ( m_methodSubscriptions[ i ]->eventName == eventName )
			{
				m_methodSubscriptions[ i ]->func( args );
			}
		}
	}
}

void EventSystem::UnSubscribeToEvent( const std::string& eventName , EventCallBackFunction eventToUnscribe )
{
	for ( int eventIndex = 0; eventIndex < m_subscriptions.size(); eventIndex++ )
	{
		if ( m_subscriptions[ eventIndex ] != nullptr )
		{
			if ( m_subscriptions[ eventIndex ]->m_eventName == eventName )
			{
				if ( m_subscriptions[ eventIndex ]->m_eventToListen == eventToUnscribe )
				{
					delete m_subscriptions[ eventIndex ];
					m_subscriptions[ eventIndex ] = nullptr;
				}
			}
		}
	}

}

