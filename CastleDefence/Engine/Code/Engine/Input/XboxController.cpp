#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // must #include Windows.h before #including Xinput.h
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility



XboxController::XboxController( int ControllerID ):m_ControllerID(ControllerID)
{
	


	
}

const KeyButtonState& XboxController::GetButtonState( XboxButtonID buttonID ) const
{
	return m_ButtonStates[buttonID];
}

void XboxController::UpdateTrigger( float& triggerValue, unsigned char rawValue )
{
	triggerValue=(1.f/255.f)* (float)rawValue;
}

void XboxController::UpdateJoyStick( AnalogJoyStick& joystick, short rawX, short rawY )
{
	float rawNormalizedX= RangeMapFloat(-32768.f,32767.f,-1.f,1.f,(float)rawX);
	float rawNormalizedY= RangeMapFloat(-32768.f,32767.f,-1.f,1.f,(float)rawY);
	joystick.UpdatePosition(rawNormalizedX,rawNormalizedY);

}

void XboxController::UpdateButton( XboxButtonID buttonId, unsigned short buttonFlags, unsigned short buttonFlag )
{
	
	if( buttonFlags & buttonFlag )
	{
		m_ButtonStates[buttonId].UpdateStatus(true);
	}
	else
	{
		m_ButtonStates[buttonId].UpdateStatus(false);
	}
}

void XboxController::Update()
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );

	int controllerid = GetControllerID();
	if( controllerid>=0 )
	{
		DWORD errorStatus = XInputGetState( GetControllerID(), &xboxControllerState );
		if( errorStatus == ERROR_SUCCESS )
		{
			m_isConnected=true;
		}
		else
			m_isConnected=false;
	}

	if( m_isConnected )
	{
		UpdateButton(XBOX_BUTTON_ID_A,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_A);
		UpdateButton(XBOX_BUTTON_ID_B,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_B);
		UpdateButton(XBOX_BUTTON_ID_X,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_X);
		UpdateButton(XBOX_BUTTON_ID_Y,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_Y);
		UpdateButton(XBOX_BUTTON_ID_BACK,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_BACK);
		UpdateButton(XBOX_BUTTON_ID_START,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_START);
		UpdateButton(XBOX_BUTTON_ID_LSHOULDER,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_RSHOULDER,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_RIGHT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_LTHUMB,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_RTHUMB,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_DPAD_RIGHT,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_UP,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton(XBOX_BUTTON_ID_DPAD_LEFT,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_DOWN,xboxControllerState.Gamepad.wButtons,XINPUT_GAMEPAD_DPAD_DOWN);

		UpdateJoyStick(m_LeftJoystick,xboxControllerState.Gamepad.sThumbLX,xboxControllerState.Gamepad.sThumbLY);
		UpdateJoyStick(m_RightJoystick,xboxControllerState.Gamepad.sThumbRX,xboxControllerState.Gamepad.sThumbRY);

		UpdateTrigger(m_leftTriggerValue,xboxControllerState.Gamepad.bLeftTrigger);
		UpdateTrigger(m_RightTriggerValue,xboxControllerState.Gamepad.bRightTrigger);
	}

	
	

}

void XboxController::Reset()
{
	 m_leftTriggerValue=0.f;
	 m_RightTriggerValue=0.f;
	 //m_ButtonStates[NUM_XBOX_BUTTONS];
	 m_LeftJoystick.Reset();
	 m_RightJoystick.Reset();
}
