#pragma once
#include "string"

class Window
{
public:
	void* m_hwnd = nullptr;
	int m_width=0;
	int m_height=0;

	explicit Window();
	~Window();

	bool Open( std::string const& title , float aspectRatio , float ratioOfHeight );
	void Close();

	inline int GetClientWidth() const { return m_width; }
	inline int GetClientHeight() const { return m_height; }

	void BeginFrame();
};
