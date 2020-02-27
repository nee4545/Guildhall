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
	CLEAR_COLOR_BIT =(1 << 0),
	CLEAR_DEPTH_BIT =(1 << 1 ),
	CLEAR_STENCIL_BIT =(1 << 2 )

};

class Camera
{
private:
	Vec2 bottom_Left;
	Vec2 top_Right;
	unsigned int m_clearMode = 0;
	Rgba8 m_clearColor;
	RenderBuffer* m_cameraUBO = nullptr;
	Vec2 m_outpitsize;
	Vec2 m_pos;


	Mat44 m_projection;
	Mat44 m_view;
	
public:
	Transform m_transform;
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	void SetProjectionPerspective( float fovDegrees ,float aspect, float nearZClip , float farZClip );

	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	Texture* GetColorTarget() const;
	void SetOutputsize( Vec2 size );
	float GetCameraHeight();
	void SetPosition( Vec2 position );
	Vec2 GetPosition() { return m_pos; }
	Vec2 GetOutputSize() { return m_outpitsize; }
	void SetOrthoViewForCameraPosition();
	void SetProjectionOrthographic( float height , float nearZ = -1.f , float farZ = 1.f );
	float GetAspectRatio() const;
	Vec2 ClientToWorldPosition( Vec2 clientPos );

	void Translate2D(Vec2 translation2D);
	void SetClearMode( unsigned int clearFlags , Rgba8 color , float depth=0.f , unsigned int stencil=0.f );

	void SetPosition( const Vec3& position );
	void Translate( const Vec3& translation );

	Rgba8 GetClearColor() const;
	RenderBuffer* UpdateAndGetUBO(RenderContext* ctx );
	Vec2 ClientToWordPosition( Vec2 clientPos );
	Mat44 GetViewMatrix();
	

	Camera()
	{
		bottom_Left=Vec2(0.0f,0.0f);
		top_Right=Vec2(0.0f,0.0f);
	}

	~Camera();

	Texture* m_texture = nullptr;
};