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

TextureView* Texture::GetOrCreateRenderTargetView()
{
	if ( m_renderTargetView )
	{
		return m_renderTargetView;
	}

	ID3D11Device* device = m_owner->m_device;
	ID3D11RenderTargetView* rtv = nullptr;
	device->CreateRenderTargetView( m_handle , nullptr , &rtv );

	if ( nullptr != rtv )
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
	delete m_depthStencilView;
	m_depthStencilView = nullptr;

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

TextureView* Texture::GetOrCreateDepthBuffer( IntVec2 dimensions,RenderContext* ctx )
{
	if ( m_depthStencilView != nullptr )
	{
		return m_depthStencilView;
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = dimensions.x;
	desc.Height = dimensions.y;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1; //MSAA
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ctx->m_device->CreateTexture2D( &desc , NULL , &m_handle );

	//Texture* temp = new Texture(  ctx , m_handle );

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ID3D11DepthStencilView* dsv = nullptr;

	memset( &dsvDesc , 0 , sizeof( D3D11_DEPTH_STENCIL_VIEW_DESC ) );

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	ctx->m_device->CreateDepthStencilView( m_handle , &dsvDesc,&dsv );

	if ( dsv != nullptr )
	{
		m_depthStencilView = new TextureView();
		m_depthStencilView->m_dsv = dsv;
	}

	return m_depthStencilView;

}

IntVec2 Texture::GetTexelSizeCoords() const
{
	return m_texelSizeCoords;
}
