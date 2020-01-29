#pragma once
#include "string"

class InputSystem;

class Window
{
public:
	void* m_hwnd = nullptr;
	InputSystem* m_input = nullptr;
	int m_width=0;
	int m_height=0;
	bool m_quitRequested = false;

	explicit Window();
	~Window();

	bool Open( std::string const& title , float aspectRatio , float ratioOfHeight );
	void Close();
	void SetInputSysten( InputSystem* input );
	void QuitWindow();


	InputSystem* GetInputSystem() const { return m_input; }

	inline int GetClientWidth() const { return m_width; }
	inline int GetClientHeight() const { return m_height; }

	void BeginFrame();
};
