#include "Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Mat44.hpp"
#define UNUSED(x) (void)(x);

void Camera::SetOrthoView( const Vec2& bottomLeft , const Vec2& topRight )
{
	/*this->bottom_Left.x = bottomLeft.x;
	this->bottom_Left.y = bottomLeft.y;
	this->top_Right.x = topRight.x;
	this->top_Right.y = topRight.y;*/

	m_projection = Mat44::CreateOrthographicProjection( Vec3( bottomLeft , 0.f ) , Vec3( topRight , 1.f ) );
}

void Camera::SetOrthoGraphicProjection( float height , float nearZ /*= -1.f */ , float farZ /*= 1.f */ )
{
	m_outpitsize.x = height * GetAspectRatio();
	m_outpitsize.y = height;

	m_projection = Mat44::CreateOrthographicProjection( Vec3( 0.f , 0.f , nearZ ) , Vec3( m_outpitsize , farZ ) );
}

void Camera::SetProjectionPerspective( float fovDegrees , float aspect , float nearZClip , float farZClip )
{
	m_projection = Mat44::CreatePerspectiveProjection( fovDegrees , aspect , nearZClip , farZClip );

}

Mat44 Camera::GetProjection()
{
	return m_projection;
}

void Camera::SetProjection( Mat44 newProjection )
{
	m_projection = newProjection;
}

Vec2 Camera::GetOrthoBottomLeft()
{
	return ClientToWordPosition2D( Vec2( 0.f , 0.f ) );
	//return bottom_Left;
}

Vec2 Camera::GetOrthoTopRight()
{
	return ClientToWordPosition2D( Vec2( 1.f , 1.f ) );
	//return top_Right;
}

Texture* Camera::GetColorTarget() const
{
	if ( m_texture != nullptr )
	{
		return m_texture;
	}

	return nullptr;
}

void Camera::SetOutputsize( Vec2 size )
{
	m_outpitsize = size;
}



float Camera::GetCameraHeight()
{
	return m_outpitsize.y;
}

void Camera::SetPosition( Vec2 position )
{
	m_transform.m_position.x = position.x;
	m_transform.m_position.y = position.y;
	SetOrthoViewForCameraPosition();

}

void Camera::SetOrthoViewForCameraPosition()
{
	bottom_Left = -m_outpitsize * 0.5f;
	top_Right = m_outpitsize * 0.5f;
	m_projection = Mat44::CreateOrthographicProjection( Vec3( bottom_Left , 0.f ) , Vec3( top_Right , 1.f ) );
}

void Camera::SetProjectionOrthographic( float height , float nearZ /*= -1.f */ , float farZ /*= 1.f */ )
{
	m_outpitsize.x = height * GetAspectRatio();
	m_outpitsize.y = height;
	UNUSED( nearZ );
	UNUSED( farZ );
	SetOrthoViewForCameraPosition();
}

float Camera::GetAspectRatio() const
{
	return m_outpitsize.x / m_outpitsize.y;
}


void Camera::Translate2D( Vec2 translation2D )
{
	m_transform.m_position.x += translation2D.x;
	m_transform.m_position.y += translation2D.y;
}

void Camera::SetClearMode( unsigned int clearFlags , Rgba8 color , float depth/*=0.f */ , unsigned int stencil/*=0.f */ )
{
	m_clearColor = color;
	m_clearMode = ( eCameraClearBitFlag ) clearFlags;
	m_clearStencil = ( float ) stencil;
	m_clearDepth = depth;
}

void Camera::SetPosition( const Vec3& position )
{
	m_transform.m_position = position;
}

void Camera::Translate( const Vec3& translation )
{
	m_transform.m_position += translation;
}



void Camera::CreateDepthStencilTarget( RenderContext* ctx )
{
	if ( m_backBuffer == nullptr )
	{
		Texture* dsTarget = new Texture();
		IntVec2 textureDimensions = m_texture->GetTexelSizeCoords();
		dsTarget->GetOrCreateDepthBuffer( textureDimensions , ctx );
		m_backBuffer = dsTarget;
	}
}

Rgba8 Camera::GetClearColor() const
{
	return m_clearColor;
}

RenderBuffer* Camera::UpdateAndGetUBO( RenderContext* ctx )
{
	if ( m_cameraUBO == nullptr )
	{
		m_cameraUBO = new RenderBuffer( ctx , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	cameraData_t cameraData;
	cameraData.projection = m_projection;

	Mat44 mat = m_transform.ToMatrix();
	cameraData.view = GetViewMatrix();

	cameraData.cameraPosition = m_transform.m_position;


	m_cameraUBO->Update( &cameraData , sizeof( cameraData ) , sizeof( cameraData ) );
	return m_cameraUBO;
}

Vec2 Camera::ClientToWordPosition2D( Vec2 clientPos , float ndcZ )
{
	Vec3 ndc;
	ndc.x = RangeMapFloat( 0.f , 1.f , -1.f , 1.f , clientPos.x );
	ndc.y = RangeMapFloat( 0.f , 1.f , -1.f , 1.f , clientPos.y );
	ndc.z = RangeMapFloat( 0.f , 1.f , 0.f , 1.f , ndcZ );

	Mat44 projection = m_projection;
	Mat44 worldToClip = projection;
	worldToClip.TransformBy( GetViewMatrix() );
	Mat44::MatrixInvert( worldToClip );
	Mat44 clipToWorld = worldToClip;
	Vec4 worldHomogeneousPt = clipToWorld.TransformHomogeneousPoint3D( Vec4( ndc.x , ndc.y , ndc.z , 1 ) );

	Vec3 worldPos;
	worldPos.x = worldHomogeneousPt.x / worldHomogeneousPt.w;
	worldPos.y = worldHomogeneousPt.y / worldHomogeneousPt.w;
	worldPos.z = worldHomogeneousPt.z / worldHomogeneousPt.w;

	return Vec2( worldPos.x , worldPos.y );

}

Vec3 Camera::ClientToWorldPosition( Vec2 clientPos , float ndcZ /*= 0 */ )
{
	Vec3 ndc;
	ndc.x = RangeMapFloat( 0.f , 1.f , -1.f , 1.f , clientPos.x );
	ndc.y = RangeMapFloat( 0.f , 1.f , -1.f , 1.f , clientPos.y );
	ndc.z = RangeMapFloat( 0.f , 1.f , 0.f , 1.f , ndcZ );

	Mat44 projection = m_projection;
	Mat44 worldToClip = projection;
	worldToClip.TransformBy( GetViewMatrix() );
	Mat44::MatrixInvert( worldToClip );
	Mat44 clipToWorld = worldToClip;
	Vec4 worldHomogeneousPt = clipToWorld.TransformHomogeneousPoint3D( Vec4( ndc.x , ndc.y , ndc.z , 1 ) );

	Vec3 worldPos;
	worldPos.x = worldHomogeneousPt.x / worldHomogeneousPt.w;
	worldPos.y = worldHomogeneousPt.y / worldHomogeneousPt.w;
	worldPos.z = worldHomogeneousPt.z / worldHomogeneousPt.w;

	return worldPos;
}

Mat44 Camera::GetViewMatrix()
{
	/*Mat44 model = m_transform.ToMatrix();
	model.MatrixInvertOrthoNormal( model );
	m_view = model;
	return m_view;*/

	Mat44 model = m_transform.ToMatrix();
	Mat44 worldToEye;

	if ( m_transform.m_convention == CONVENSION_XEAST_YNORTH_ZUP )
	{
		worldToEye.SetBasisVectors3D( Vec3( 0.f , 0.f , -1.f ) , Vec3( -1.f , 0.f , 0.f ) , Vec3( 0.f , 1.f , 0.f ) );
	}

	Mat44::MatrixTranspose( worldToEye );
	model.TransformBy( worldToEye );
	model.MatrixInvertOrthoNormal( model );
	m_view = model;
	return m_view;
}

Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;

	//delete m_texture;
	m_texture = nullptr;

	if ( m_backBuffer != nullptr )
	{
		delete m_backBuffer;
		m_backBuffer = nullptr;
	}
}

