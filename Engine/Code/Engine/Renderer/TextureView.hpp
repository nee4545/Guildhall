#pragma once

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;
struct ID3D11DepthStencilView;

class TextureView
{
public:

	TextureView() { m_handle = nullptr; };
	~TextureView();
	ID3D11RenderTargetView* GetRTVHandle() const { return m_rtv; }
	ID3D11ShaderResourceView* GetSRV() const { return m_srv; }

public:

	union 
	{
		ID3D11Resource* m_handle;
		ID3D11RenderTargetView* m_rtv;
		ID3D11ShaderResourceView* m_srv;
		ID3D11DepthStencilView* m_dsv;
	};
	

	ID3D11DepthStencilView* GetDsvHandle() const { return m_dsv; }

};