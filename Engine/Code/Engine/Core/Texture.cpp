#include "Engine/Core/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/D3D11Common.hpp"
#include "Engine/Renderer/TextureView.hpp"


Texture::Texture( const char* filePath, unsigned int texID, IntVec2 size ):imageFilePath(filePath),textureID(texID),m_dimensions(size)
{
	
}



Texture::Texture( RenderContext* ctx , ID3D11Texture2D* handle ):m_owner(ctx),m_handle(handle)
{
	D3D11_TEXTURE2D_DESC desc;
	handle->GetDesc( &desc );

	m_texelSizeCoords = IntVec2( desc.Width , desc.Height );
}

Texture::Texture( const char* filePath , RenderContext* ctx , ID3D11Texture2D* handle )
{
	imageFilePath = filePath;
	m_owner = ctx;
	m_handle = handle;

	D3D11_TEXTURE2D_DESC desc;
	m_handle->GetDesc( &desc );
	m_dimensions.x = desc.Width;
	m_dimensions.y = desc.Height;
}

TextureView* Texture::GetRenderTargetView()
{

	if ( m_renderTargetView )
	{
		return m_renderTargetView;
	}

	ID3D11Device* dev = m_owner->m_device;
	ID3D11RenderTargetView* rtv = nullptr;
	dev->CreateRenderTargetView( m_handle , nullptr , &rtv );

	if ( rtv!= nullptr )
	{
		m_renderTargetView = new TextureView();
		m_renderTargetView->m_rtv = rtv;
	}


	return m_renderTargetView;
}

TextureView* Texture::GetOrCreateShaderResourceView()
{
	if ( m_shaderResourceView )
	{
		return m_shaderResourceView;
	}

	ID3D11Device* dev = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;

	dev->CreateShaderResourceView( m_handle , nullptr , &srv );

	if ( srv != nullptr )
	{
		m_shaderResourceView = new TextureView();
		m_shaderResourceView->m_srv = srv;
	}

	return m_shaderResourceView;
}

Texture::~Texture()
{
	delete m_renderTargetView;
	m_renderTargetView = nullptr;

	delete m_shaderResourceView;
	m_shaderResourceView = nullptr;

	DX_SAFE_RELEASE( m_handle );

}

unsigned int Texture::GetTextureID() const
{
	return textureID;
}

IntVec2 Texture::GetTexelSizeCoords() const
{
	return m_texelSizeCoords;
}
