#pragma once
#include "Engine/Input/XboxController.hpp"

constexpr int MAX_XBOX_CONTROLLERS=4;
constexpr int MAX_KEYBOARD_STATES=256;


class InputSystem
{
public:
	InputSystem();
	~InputSystem(){};

	void StartUp();
	void BeginFrame();
	void EndFrame();
	void ShutDown();

	bool IsKeyPressed( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

	bool WasLeftMouseButtonJustPressed() const;
	bool WasLeftMouseButtonJustReleased() const;
	bool IsLeftMouseButtonPressed() const;

	bool WasRightMouseButtonJustPressed() const;
	bool WasRightMouseButtonJustReleased()const;
	bool IsRightMouseButtonPressed()const;

	bool HandleKeyPressed(unsigned char keyCode);
	bool HandleKeyReleased(unsigned char keyCode);
	bool HandleLeftMouseButtonPressed();
	bool HandleLeftMouseButtonReleased();
	bool HandelRightMouseButtonPressed();
	bool HandleRightMouseButtonReleased();


	const XboxController& GetXboxController(int ControllerID);
	Vec2 GetCurrentMousePosition() { return m_mouseNormalizedClientPos; }

	void UpdateMouse();

private:

	KeyButtonState m_keyStates[MAX_KEYBOARD_STATES];
	KeyButtonState m_leftMousebutton;
	KeyButtonState m_rightMousebutton;

	XboxController m_Cotrollers[MAX_XBOX_CONTROLLERS] =
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};

	Vec2 m_mouseNormalizedClientPos;
	


};