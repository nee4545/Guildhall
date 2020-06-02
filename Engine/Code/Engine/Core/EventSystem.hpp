#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include <vector>
#include <map >
#include <functional>

typedef NamedProperties EventArgs;
typedef bool( *EventCallBackFunction )( EventArgs& args );


struct MethodSubscriptions
{
public:
	std::string eventName;
	void* objId = nullptr;
	std::function<bool( EventArgs& )> func = nullptr;
};

struct EventSubscription
{
	std::string m_eventName;
	EventCallBackFunction m_eventToListen = nullptr;
};

class EventSystem
{
private:
	std::vector<EventSubscription*> m_subscriptions;
	std::vector<MethodSubscriptions*> m_methodSubscriptions;

public:
	void StartUp();
	void ShutDown();
	void SubscribeToEvent( const std::string& eventName , EventCallBackFunction eventToListen );
	void FireEvent( const std::string& eventName , EventArgs& args );
	void UnSubscribeToEvent( const std::string& eventName , EventCallBackFunction eventToUnscribe );

	template<typename OBJ_TYPE>
	void SubscribeToMethod( const std::string& eventName , OBJ_TYPE* obj , bool ( OBJ_TYPE::* mcb )( EventArgs& args ) )
	{
		MethodSubscriptions* newSubscription = new MethodSubscriptions();
		newSubscription->eventName = eventName;
		newSubscription->objId = obj;
		newSubscription->func = [ = ] ( EventArgs& args ) -> bool { return ( obj->*mcb )( args ); };

		m_methodSubscriptions.push_back( newSubscription );
	};

	template<typename OBJ_TYPE>
	void UnsubscribeMethod( const std::string& eventName , OBJ_TYPE* obj )
	{
		for ( int i = 0; i < m_methodSubscriptions.size(); i++ )
		{
			if ( m_methodSubscriptions[ i ] != nullptr )
			{
				if ( m_methodSubscriptions[ i ]->eventName == eventName )
				{
					if ( m_methodSubscriptions[ i ]->objId == obj )
					{
						delete m_methodSubscriptions[ i ];
						m_methodSubscriptions[ i ] = nullptr;
					}
				}
			}
		}
	}

};