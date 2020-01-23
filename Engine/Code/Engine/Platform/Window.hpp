#pragma once
#include "string"

class Window
{
public:
	void* m_hwnd = nullptr;

	Window();
	~Window();

	bool Open( std::string const& title , float aspectRatio , float ratioOfHeight );
	void Close();

	void BeginFrame();
};
