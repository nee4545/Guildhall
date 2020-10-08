#pragma once
#include "AnalogJoyStick.hpp"
#include "KeyButtonState.hpp"

enum XboxButtonID
{
	XBOX_BUTTON_ID_INVALID = -1,

	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,
	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,
	XBOX_BUTTON_ID_LSHOULDER,
	XBOX_BUTTON_ID_RSHOULDER,
	XBOX_BUTTON_ID_LTHUMB,
	XBOX_BUTTON_ID_RTHUMB,
	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_DPAD_LEFT,
	XBOX_BUTTON_ID_DPAD_DOWN,

	NUM_XBOX_BUTTONS
};


class XboxController
{
	friend class InputSystem;

public:

	explicit XboxController(int ControllerID);
	~XboxController(){};


	bool IsCOnnected() const {return m_isConnected;}
	int  GetControllerID() const {return m_ControllerID;}
	const AnalogJoyStick& GetLeftJoystick() const {return m_LeftJoystick;}
	const AnalogJoyStick& GetRightJoystick() const {return m_RightJoystick;}
	float GetLeftTrigger() const {return m_leftTriggerValue;}
	float GetRightTrigger() const {return m_RightTriggerValue;}
	const KeyButtonState& GetButtonState(XboxButtonID buttonID)const;


private:

	void Update();
	void Reset();
	void UpdateTrigger(float &triggerValue, unsigned char rawValue);
	void UpdateJoyStick(AnalogJoyStick& joystick, short rawX, short rawY);
	void UpdateButton(XboxButtonID buttonId,unsigned short buttonFlags,unsigned short buttonFlag);


private:
	const int m_ControllerID=-1;
	bool m_isConnected=false;

	float m_leftTriggerValue=0.f;
	float m_RightTriggerValue=0.f;
	KeyButtonState m_ButtonStates[NUM_XBOX_BUTTONS];
	AnalogJoyStick m_LeftJoystick = AnalogJoyStick(0.3f,0.9f);
	AnalogJoyStick m_RightJoystick = AnalogJoyStick(0.3f,0.9f);



};