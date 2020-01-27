#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/D3D11Common.hpp"

SwapChain::SwapChain( RenderContext* owner , IDXGISwapChain* handle ):m_owner(owner),m_handle(handle)
{

}

SwapChain::~SwapChain()
{
	DX_SAFE_RELEASE( m_handle );
}

void SwapChain::Present( int vsync /*= 0 */ )
{
	m_handle->Present( 0 , 0 );
}

