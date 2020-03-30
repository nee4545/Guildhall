#pragma once
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Transform.hpp"

class RenderContext;
class Clock;
class Timer;
class Camera;
class Texture;
class GPUMesh;

enum eDebugRenderMode
{
	DEBUG_RENDER_ALWAYS ,          // what is rendered always shows up
	DEBUG_RENDER_USE_DEPTH ,       // respect the depth buffer
	DEBUG_RENDER_XRAY ,            // renders twice - once darker when it shoudl be hidden, and once more saturated when it should appear
};

enum eObjectTypes
{
	OBJECT_INVALID,
	OBJECT_POINT,
	OBJECT_LINE,
	OBJECT_ARROW,
	OBJECT_QUAD,

};

enum eDebugRenderSpace
{
	DEBUG_RENDER_SCREEN,
	DEBUG_RENDER_WORLD,
};


class DebugRenderObject
{
public:

	DebugRenderObject() {};
	~DebugRenderObject() {};

	DebugRenderObject(eObjectTypes type, eDebugRenderMode mode , eDebugRenderSpace space , float duration , bool isBillboard = false );

	float m_duration = 0.f;
	Timer* m_timer = nullptr;
	eDebugRenderMode m_renderMode = DEBUG_RENDER_ALWAYS;
	eObjectTypes m_objectType;
	eDebugRenderSpace m_space;
	Transform m_transform;

	Rgba8 m_startColor;
	Rgba8 m_endColor;
	Rgba8 m_color;

	bool m_isBillboard = false;
	bool m_isGarbage = false;
	bool m_needsColorLerp = false;
	void Update();
	void MarkForDestroy();

};

class DebugPoint : public DebugRenderObject
{
public:
	DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space, Vec3 pos, float size, Rgba8 color, float duration, bool isBillboard = false);
	DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space , Transform transform , float size , Rgba8 color , float duration , bool isBillboard = false );
	DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 pos , float size , Rgba8 startColor, Rgba8 endColor , float duration , bool isBillboard = false );
	Vec3 m_position;
	float m_size;
};

class DebugLine : public DebugRenderObject
{
public:
	DebugLine( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end ,Rgba8 color,float lineThickness, float duration , bool isBillboard = false );
	DebugLine( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end , Rgba8 p0StartColor , Rgba8 P0endColor ,Rgba8 p1StartColor, Rgba8 p1EndColor, float lineThickness , float duration , bool isBillboard = false );

	Vec3 m_startPos;
	Vec3 m_endPos;
	float m_lineThickness;

	Rgba8 m_p0StartColor;
	Rgba8 m_p0EndColor;
	Rgba8 m_p1StartColor;
	Rgba8 m_p1EndColor;

};

class DebugArrow : public DebugRenderObject
{
public:
	DebugArrow( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end , Rgba8 color , float lineThickness , float tipHeight , float duration = -1.f , bool isBillboard = false );
	DebugArrow( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end , Rgba8 p0startColor, Rgba8 p0endColor, Rgba8 p1startColor, Rgba8 p1endColor , float lineThickness , float tipHeight , float duration = -1.f , bool isBillboard = false );

	Vec3 m_startPos;
	Vec3 m_endPos;
	float m_lineThickness;
	float m_tipHeight;

	Rgba8 m_p0StartColor;
	Rgba8 m_p0EndColor;
	Rgba8 m_p1StartColor;
	Rgba8 m_p1EndColor;

};

class DebugQuad : public DebugRenderObject
{
public:
	DebugQuad( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , float duration = -1.f , bool isBillboard = false );
	DebugQuad( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 startColor ,Rgba8 endColor, float duration = -1.f , bool isBillboard = false );
	DebugQuad( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color ,Texture* tex,AABB2 uvs, float duration = -1.f , bool isBillboard = false );

	Vec3 m_p0;
	Vec3 m_p1;
	Vec3 m_p2;
	Vec3 m_p3;

	Texture* m_texture = nullptr;
	AABB2 m_uvs = AABB2(Vec2(0.f,0.f),Vec2(1.f,1.f));

};

class DebugRenderSystem
{

public:
	DebugRenderSystem() {};
	~DebugRenderSystem() {};
	void SystemStartUp();
	void SystemShutDown();
	void TakeWorldCamera( Camera* cam );
	void TakeScreenTexture( Texture* tex );
	void BeginFrame();
	void Update();
	void CleanUp();

	void DebugRenderWorldToCamera( Camera* cam );
	void DebugRenderToScreen( Texture* output );

	void DebugRenderSetScreenHeight( float height ); // default to 1080.0f when system starts up.  Meaning (0,0) should always be bottom left, (aspect * height, height) is top right
	AABB2 DebugGetScreenBounds();

public:
	std::vector<DebugRenderObject*> m_screenObjects;
	std::vector<DebugRenderObject*> m_worldObjects;
	Camera* m_camera = nullptr;
	Texture* m_output = nullptr;
	RenderContext* m_context = nullptr;
	float m_defaultScreenHeight = 1080.f;
	static DebugRenderSystem* sDebugRenderer;

private:
	GPUMesh* point = nullptr;
};


//World Renders

void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 color, float duration , eDebugRenderMode mode = DEBUG_RENDER_ALWAYS );
void DebugAddWorldPoint( Transform transform , float size , Rgba8 color , float duration , eDebugRenderMode mode = DEBUG_RENDER_ALWAYS );
void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 startColor, Rgba8 endColor , float duration , eDebugRenderMode mode = DEBUG_RENDER_ALWAYS );

void DebugAddWorldLine( Vec3 p0 , Vec3 p1, Rgba8 p0_start_color , Rgba8 p0_end_color , Rgba8 p1_start_color , Rgba8 p1_end_color ,float lineThickness,float duration ,eDebugRenderMode mode = DEBUG_RENDER_ALWAYS );
void DebugAddWorldLine( Vec3 start , Vec3 end , Rgba8 color , float lineThickness , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

void DebugAddWorldArrow( Vec3 p0, Vec3 p1 , Rgba8 p0_start_color , Rgba8 p0_end_color , Rgba8 p1_start_color , Rgba8 p1_end_color ,float duration, float thickness ,eDebugRenderMode mode=DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldArrow( Vec3 start , Vec3 end , Rgba8 color , float duration ,float thickness, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color, Texture* tex, float duration, AABB2 uvs = AABB2( Vec2( 0.f , 0.f ) , Vec2( 1.f , 1.f ) ) , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

//Screen Renders
void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 start_color , Rgba8 end_color , float duration );
void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 color , float duration = 0.0f );
void DebugAddScreenPoint( Vec2 pos , Rgba8 color ); // assumed size;

void DebugAddScreenLine( Vec2 p0 , Rgba8 p0_start_color , Rgba8 p0_end_color ,Vec2 p1 , Rgba8 p1_start_color , Rgba8 p1_end_color, float lineThickness ,float duration );
void DebugAddScreenLine( Vec2 p0 , Vec2 p1 , Rgba8 color ,float lineThickness, float duration = 0.0f );

void DebugAddScreenArrow( Vec2 p0 , Rgba8 p0_start_color , Rgba8 p0_end_color , Vec2 p1 , Rgba8 p1_start_color , Rgba8 p1_end_color ,float lineThickness,float tipHeight, float duration );
void DebugAddScreenArrow( Vec2 p0 , Vec2 p1 , Rgba8 color ,float lineThickness,float tipHeight, float duration = 0.0f );

void DebugAddScreenQuad( AABB2 bounds , Rgba8 start_color , Rgba8 end_color , float duration );
void DebugAddScreenQuad( AABB2 bounds , Rgba8 color , float duration = 0.0f );
void DebugAddScreenTexturedQuad( AABB2 bounds , Texture* tex , AABB2 uvs , Rgba8 tint , float duration = 0.0f );








