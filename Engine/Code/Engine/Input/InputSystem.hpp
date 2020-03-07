#pragma once
#include "Engine/Input/XboxController.hpp"
#include <queue>

constexpr int MAX_XBOX_CONTROLLERS=4;
constexpr int MAX_KEYBOARD_STATES=256;

enum eMousePositionMode
{
	MODE_ABSOLUTE=0,
	MODE_RELATIVE
};


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

	void PushCharacter( char character );
	bool PopCharacter( char* outCharacter );

	void HideSystemCursor();
	void ShowSystemCursor();
	void UnClipSystemCursor();
	void ClipSystemCursor();
	void UpdateRelativeMode();
	void SetCursorMode( eMousePositionMode mode );
	eMousePositionMode GetCursorMode();

	const XboxController& GetXboxController(int ControllerID);
	Vec2 GetCurrentMousePosition() { return m_mouseNormalizedClientPos; }
	Vec2 m_relativeMovement;


	void UpdateMouse();
	std::queue<char> m_characters;
	int GetMouseWheelData();
	void SetMouseWheelData( int data );

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
	int m_mouseWheelData = 0;
	Vec2 m_positionLastFrame;
	eMousePositionMode m_mode = MODE_ABSOLUTE;
	int m_hideCursorIndex = 0;
	


};