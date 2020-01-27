#pragma once


struct IDXGISwapChain;
class RenderContext;

class SwapChain
{
public:
	SwapChain( RenderContext* owner , IDXGISwapChain* handle );
	~SwapChain();

	void Present( int vsync = 0 );

public:

	RenderContext* m_owner;
	IDXGISwapChain* m_handle;
};