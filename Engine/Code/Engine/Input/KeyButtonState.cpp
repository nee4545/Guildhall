#include "Engine/Input/KeyButtonState.hpp"

void KeyButtonState::UpdateStatus( bool isNowPressed )
{
	m_wasPressedLastFrame=m_isPressed;
	m_isPressed=isNowPressed;
}

bool KeyButtonState::IsPressed() const
{
	return m_isPressed;
}

bool KeyButtonState::WasJustPressed() const
{
	if( m_isPressed==true && m_wasPressedLastFrame==false)
	{
		return true;
	}
	else
	{
		return false;
	}

	/*if( m_isPressed !=m_wasPressedLastFrame )
	{
		return true;
	}
	return false;*/
}

bool KeyButtonState::WasJustReleased() const
{
	if( m_wasPressedLastFrame ==true && m_isPressed==false )
	{
		return true;
	}
	else
	{ 
		return false;
	}

	/*if( m_wasPressedLastFrame != m_isPressed )
	{
		return true;
	}
	return false;*/
}

void KeyButtonState::Reset()
{
	m_isPressed=false;
	m_wasPressedLastFrame=false;
}




