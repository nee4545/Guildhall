#include "Camera.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#define UNUSED(x) (void)(x);

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	this->bottom_Left.x=bottomLeft.x;
	this->bottom_Left.y=bottomLeft.y;
	this->top_Right.x=topRight.x;
	this->top_Right.y=topRight.y;
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return bottom_Left;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return top_Right;
}

Texture* Camera::GetColorTarget() const
{
	if ( m_texture != nullptr )
	{
		return m_texture;
	}

	return nullptr;
}

void Camera::Translate2D( Vec2 translation2D )
{
	bottom_Left+=translation2D;
	top_Right+=translation2D;
}

void Camera::SetClearMode( unsigned int clearFlags , Rgba8 color , float depth/*=0.f */ , unsigned int stencil/*=0.f */ )
{
	m_clearColor = color;
	m_clearMode = clearFlags;
	UNUSED( depth );
	UNUSED( stencil );
}

Rgba8 Camera::GetClearColor() const
{
	return m_clearColor;
}

RenderBuffer* Camera::UpdateAndGetUBO(RenderContext* ctx )
{
	if ( m_cameraUBO == nullptr )
	{
		m_cameraUBO = new RenderBuffer( ctx , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	cameraData_t cameraData;
	cameraData.orthoMin = bottom_Left;
	cameraData.orthoMax = top_Right;

	m_cameraUBO->Update( &cameraData , sizeof( cameraData ) , sizeof( cameraData ));

	return m_cameraUBO;
}

Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;
}

