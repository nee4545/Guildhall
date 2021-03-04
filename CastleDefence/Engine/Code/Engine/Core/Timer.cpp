#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

Timer::Timer()
{

}

void Timer::SetSeconds( Clock* clock , double timeToWait )
{
	m_clock = clock;
	m_durationSeconds = timeToWait;
	m_startSeconds = clock->GetTotalElapsedSeconds();
}

void Timer::SetSeconds( double timeToWait )
{
	if ( m_clock == nullptr )
	{
		m_clock = &Clock::gMasterClock;
	}

	m_durationSeconds = timeToWait;
	m_startSeconds = m_clock->GetTotalElapsedSeconds();
}

void Timer::Reset()
{
	m_startSeconds = m_clock->GetTotalElapsedSeconds();
}

void Timer::Stop()
{
	m_durationSeconds = -1.0;
}

double Timer::GetElapsedSeconds() const
{
	return m_durationSeconds - GetSecondsRemaining();
}

double Timer::GetSecondsRemaining() const
{
	double currentTime = m_clock->GetTotalElapsedSeconds();
	return m_startSeconds + m_durationSeconds - currentTime;
}

bool Timer::HasElapsed() const
{
	double currentTime = m_clock->GetTotalElapsedSeconds();
	return ( currentTime > ( m_startSeconds + m_durationSeconds ) );
}

bool Timer::CheckAndDecrement()
{
	if ( HasElapsed() )
	{
		m_startSeconds += m_durationSeconds;
		return true;
	}
	else
	{
		return false;
	}
}

int Timer::CheckAndDecrementAll()
{
	double currentTime = m_clock->GetTotalElapsedSeconds();

	if ( currentTime < m_startSeconds )
	{
		return 0;
	}

	int numIntervals = ( int ) (( currentTime - m_startSeconds ) / m_durationSeconds);

	m_startSeconds += ( numIntervals * m_durationSeconds );

	return numIntervals;
}

bool Timer::CheckAndReset()
{
	if ( HasElapsed() )
	{
		// remove an interval of time
		m_startSeconds = m_clock->GetTotalElapsedSeconds();
		return true;
	}
	else
	{
		return false;
	}
}

bool Timer::IsRunning() const
{
	return m_clock->IsPaused();
}

