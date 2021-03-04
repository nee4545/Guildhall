#include "Engine/Core/EngineCommon.hpp"
#include"Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/AABB2.hpp"

class Window;

extern Window* g_theWindow;

InputSystem::InputSystem()
{
	
}

void InputSystem::StartUp()
{

}

void InputSystem::BeginFrame()
{
	m_Cotrollers[0].Update();
	m_Cotrollers[1].Update();
	m_Cotrollers[2].Update();
	m_Cotrollers[3].Update();
}

void InputSystem::EndFrame()
{
	for( int i=0; i< MAX_KEYBOARD_STATES; i++ )
	{
		m_keyStates[i].m_wasPressedLastFrame=m_keyStates[i].m_isPressed;
	}

	m_leftMousebutton.m_wasPressedLastFrame = m_leftMousebutton.m_isPressed;
	m_rightMousebutton.m_wasPressedLastFrame = m_rightMousebutton.m_isPressed;

	for ( int index = 0; index < m_characters.size(); index++ )
	{
		m_characters.pop();
	}
	m_mouseWheelData = 0;

	m_relativeMovement = Vec2( 0.f , 0.f );
}

void InputSystem::ShutDown()
{

}

bool InputSystem::IsKeyPressed( unsigned char keyCode ) const
{
	return m_keyStates[keyCode].IsPressed();
}

bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const
{
	return m_keyStates[keyCode].WasJustPressed();
}



bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const
{
	return m_keyStates[keyCode].WasJustReleased();
}



bool InputSystem::WasLeftMouseButtonJustPressed() const
{
	return m_leftMousebutton.WasJustPressed();
}

bool InputSystem::WasLeftMouseButtonJustReleased() const
{
	return m_leftMousebutton.WasJustReleased();
}

bool InputSystem::IsLeftMouseButtonPressed() const
{
	return m_leftMousebutton.IsPressed();
}

bool InputSystem::WasRightMouseButtonJustPressed() const
{
	return m_rightMousebutton.WasJustPressed();
}

bool InputSystem::WasRightMouseButtonJustReleased() const
{
	return m_rightMousebutton.WasJustReleased();
}

bool InputSystem::IsRightMouseButtonPressed() const
{
	return m_rightMousebutton.IsPressed();
}

bool InputSystem::HandleKeyPressed( unsigned char keyCode )
{
    m_keyStates[keyCode].UpdateStatus(true);
	return true;
}

bool InputSystem::HandleKeyReleased( unsigned char keyCode )
{
	m_keyStates[keyCode].UpdateStatus(false);
	return true;
}

bool InputSystem::HandleLeftMouseButtonPressed()
{
	m_leftMousebutton.UpdateStatus( true );
	return true;
}

bool InputSystem::HandleLeftMouseButtonReleased()
{
	m_leftMousebutton.UpdateStatus( false );
	return true;
}

bool InputSystem::HandelRightMouseButtonPressed()
{
	m_rightMousebutton.UpdateStatus( true );
	return true;
}

bool InputSystem::HandleRightMouseButtonReleased()
{
	m_rightMousebutton.UpdateStatus( false );
	return true;
}

void InputSystem::PushCharacter( char character )
{
	m_characters.push( character );
}

bool InputSystem::PopCharacter( char* outCharacter )
{
	if ( !m_characters.empty() )
	{
		*outCharacter = m_characters.front();
		m_characters.pop();
		return true;
	}
	else
	{
		return false;
	}
}

void InputSystem::HideSystemCursor()
{
	while ( ShowCursor( FALSE ) >= 0 );
	
}

void InputSystem::ShowSystemCursor()
{
	while ( ShowCursor( TRUE ) <= 0 );
}

void InputSystem::UnClipSystemCursor()
{
	HWND hwnd;
	RECT desktopRect;
	hwnd = GetDesktopWindow();
	GetClientRect( hwnd , &desktopRect );

	ClipCursor( &desktopRect );

}

void InputSystem::ClipSystemCursor()
{
	HWND hwnd = ( HWND ) g_theWindow->m_hwnd;
	RECT clientRect;
	GetWindowRect( hwnd , &clientRect );

	ClipCursor( &clientRect );
	
}



void InputSystem::UpdateRelativeMode()
{
	POINT curPos ;
	GetCursorPos( &curPos );
	Vec2 positionThisFrame = Vec2((float)curPos.x,(float)curPos.y);
	m_relativeMovement = positionThisFrame - m_positionLastFrame;

	Vec2 windowCenter = g_theWindow->GetCentre(); 
	SetCursorPos( (int)windowCenter.x,(int)windowCenter.y );

	POINT point ;
	GetCursorPos( &point );
	windowCenter = Vec2( (float)point.x , (float)point.y );

	m_positionLastFrame = windowCenter;
}

void InputSystem::SetCursorMode( eMousePositionMode mode )
{
	m_mode = mode;
	switch ( mode )
	{
		case MODE_RELATIVE:
		{
			Vec2 windowCenter = g_theWindow->GetCentre(); 
			SetCursorPos( (int)windowCenter.x,(int)windowCenter.y );
			m_positionLastFrame = windowCenter;
		}
		default:
			break;
	}
}

eMousePositionMode InputSystem::GetCursorMode()
{
	return m_mode;
}

const XboxController& InputSystem::GetXboxController( int ControllerID )
{
	return m_Cotrollers[ControllerID];
}

void InputSystem::UpdateMouse()
{
	HWND hwnd = (HWND)g_theWindow->m_hwnd;
	POINT mousePos;
	GetCursorPos( &mousePos );
	ScreenToClient( hwnd, &mousePos );
	Vec2 mouseClientPos = Vec2( (float)mousePos.x, (float)mousePos.y );

	RECT clientRect;
	GetClientRect( hwnd, &clientRect );
	AABB2 clientBounds = AABB2( (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom );
	m_mouseNormalizedClientPos = clientBounds.GetUVForPoint( mouseClientPos );
	m_mouseNormalizedClientPos.y = 1.f - m_mouseNormalizedClientPos.y;
	

}

int InputSystem::GetMouseWheelData()
{
	return m_mouseWheelData;
}

void InputSystem::SetMouseWheelData( int data )
{
	m_mouseWheelData = data;
}

