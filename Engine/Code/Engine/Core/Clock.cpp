#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Clock Clock::gMasterClock = Clock(nullptr);

Clock::Clock()
{
	m_parent = &gMasterClock;
	gMasterClock.AddChild( this );
}

Clock::Clock( Clock* parent )
{
	if ( parent != nullptr )
	{
		this->m_parent = parent;
		parent->AddChild( this );
	}
}

Clock::~Clock()
{
	//All children are deleted if master is deleted
	if ( this == &gMasterClock )
	{
		m_children.clear();
		return;
	}

	//Re parent children to parent/master
	for ( int i = 0; i < m_children.size(); i++ )
	{
		if(m_children[i]!=nullptr )
		m_children[ i ]->SetParent( m_parent );
	}
}

void Clock::Update( double deltaSeconds )
{
	if ( isPaused )
	{
		deltaSeconds = 0.f;
	}

	deltaSeconds = deltaSeconds * m_scale;
	m_currentTimeSeconds += deltaSeconds;
	m_frameDeltaSeconds = deltaSeconds;

	for ( int i = 0; i < m_children.size(); i++ )
	{
		m_children[ i ]->Update( deltaSeconds );
	}
	
}

void Clock::Reset()
{
	m_currentTimeSeconds = 0.0;
	m_frameDeltaSeconds = 0.0;
}

void Clock::Pause()
{
	isPaused = true;
}

void Clock::Resume()
{
	isPaused = false;
}

void Clock::SetScale( double scale )
{
	this->m_scale = scale;
}

double Clock::GetTotalElapsedSeconds() const
{
	return m_currentTimeSeconds;
}

double Clock::GetLastDeltaSeconds() const
{
	return m_frameDeltaSeconds;
}

double Clock::GetScale() const
{
	return m_scale;
}

bool Clock::IsPaused() const
{
	return isPaused;
}

void Clock::SetParent( Clock* clock )
{
	if ( clock == nullptr )
	{
		m_parent = &gMasterClock;
		gMasterClock.AddChild( clock );
		return;
	}

	m_parent = clock;
	clock->AddChild( this );
}

void Clock::AddChild( Clock* clock )
{
	ASSERT_OR_DIE( clock != nullptr , "Must pass a valid clock" );
	m_children.push_back( clock );
}

void Clock::SystemStartup()
{
	gMasterClock.Reset();
}

void Clock::SystemShutdown()
{

}

void Clock::BeginFrame()
{
	static double timePreviousFrame = GetCurrentTimeSeconds();
	double timeThisFrame = GetCurrentTimeSeconds();

	double dt = timeThisFrame - timePreviousFrame;
	timePreviousFrame = timeThisFrame;

	gMasterClock.Update( dt );
}	

Clock* Clock::GetMaster()
{
	return &gMasterClock;
}


