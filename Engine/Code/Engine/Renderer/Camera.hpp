#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Transform.hpp"

class Texture;
class TextureView;
class RenderBuffer;
class RenderContext;

enum  eCameraClearBitFlag : unsigned int
{
	CLEAR_COLOR_BIT = ( 1 << 0 ) ,
	CLEAR_DEPTH_BIT = ( 1 << 1 ) ,
	CLEAR_STENCIL_BIT = ( 1 << 2 )

};

class Camera
{
private:
	Vec2 bottom_Left;
	Vec2 top_Right;
	Rgba8 m_clearColor;
	RenderBuffer* m_cameraUBO = nullptr;
	Vec2 m_outpitsize;
	Vec2 m_pos;

	float m_clearStencil = 0.f;
	float m_clearDepth = 1.f;

	Mat44 m_projection;
	Mat44 m_view;

public:
	unsigned int m_clearMode = 0;
	Transform m_transform;
	void SetOrthoView( const Vec2& bottomLeft , const Vec2& topRight );
	void SetOrthoGraphicProjection( float height , float nearZ = -1.f , float farZ = 1.f );
	void SetProjectionPerspective( float fovDegrees , float aspect , float nearZClip , float farZClip );

	Mat44 GetProjection();
	void SetProjection( Mat44 newProjection );

	Vec2 GetOrthoBottomLeft();
	Vec2 GetOrthoTopRight();
	Texture* GetColorTarget() const;
	void SetOutputsize( Vec2 size );
	float GetCameraHeight();
	void SetPosition( Vec2 position );
	Vec2 GetPosition() { return m_pos; }
	Vec2 GetOutputSize() { return m_outpitsize; }
	void SetOrthoViewForCameraPosition();
	void SetProjectionOrthographic( float height , float nearZ = -1.f , float farZ = 1.f );
	float GetAspectRatio() const;

	void Translate2D( Vec2 translation2D );
	void SetClearMode( unsigned int clearFlags , Rgba8 color , float depth = 0.f , unsigned int stencil = 0.f );

	void SetPosition( const Vec3& position );
	void Translate( const Vec3& translation );
	void CreateDepthStencilTarget( RenderContext* ctx );

	Rgba8 GetClearColor() const;
	RenderBuffer* UpdateAndGetUBO( RenderContext* ctx );
	Vec2 ClientToWordPosition2D( Vec2 clientPos , float ndcZ = 0 );
	Vec3 ClientToWorldPosition( Vec2 clientPos , float ndcZ = 0 );
	Mat44 GetViewMatrix();



	Camera()
	{
		bottom_Left = Vec2( 0.0f , 0.0f );
		top_Right = Vec2( 0.0f , 0.0f );
	}

	~Camera();

	Texture* m_texture = nullptr;
	Texture* m_backBuffer = nullptr;
};