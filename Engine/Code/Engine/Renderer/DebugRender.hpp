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
	bool m_isBillboard = false;
	bool m_isGarbage = false;
	void Update();
	void MarkForDestroy();

};

class DebugPoint : public DebugRenderObject
{
public:
	DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space, Vec3 pos, float size, Rgba8 color, float duration, bool isBillboard = false);
	DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space , Transform transform , float size , Rgba8 color , float duration , bool isBillboard = false );
	Vec3 m_position;
	float m_size;
	Rgba8 m_color;

	Rgba8 m_startColor;
	Rgba8 m_endColor;

	bool m_needsColorLerp = false;
};

class DebugLine : public DebugRenderObject
{
public:
	DebugLine( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end ,Rgba8 color,float lineThickness, float duration , bool isBillboard = false );

	Vec3 m_startPos;
	Vec3 m_endPos;
	Rgba8 m_color;
	float m_lineThickness;

};

class DebugArrow : public DebugRenderObject
{
public:
	DebugArrow( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end ,Rgba8 m_color, float lineThickness ,float tipHeight, float duration=-1.f , bool isBillboard = false );

	Vec3 m_startPos;
	Vec3 m_endPos;
	Rgba8 m_color;
	float m_lineThickness;
	float m_tipHeight;

};

class DebugQuad : public DebugRenderObject
{
public:
	DebugQuad( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , float duration = -1.f , bool isBillboard = false );

	Vec3 m_p0;
	Vec3 m_p1;
	Vec3 m_p2;
	Vec3 m_p3;

	Rgba8 m_color;
	Texture* m_texture = nullptr;
	AABB2 m_uvs;

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

//Points
void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 color, float duration , eDebugRenderMode mode = DEBUG_RENDER_ALWAYS );
void DebugAddWorldPoint( Transform transform , float size , Rgba8 color , float duration , eDebugRenderMode mode = DEBUG_RENDER_ALWAYS );

// lines
void DebugAddWorldLine( Vec3 p0 , Rgba8 p0_start_color , Rgba8 p0_end_color ,Vec3 p1 , Rgba8 p1_start_color , Rgba8 p1_end_color ,float duration ,eDebugRenderMode mode = DEBUG_RENDER_ALWAYS );
void DebugAddWorldLine( Vec3 start , Vec3 end , Rgba8 color , float lineThickness , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

//Arrows
void DebugAddWorldArrow( Vec3 p0 , Rgba8 p0_start_color , Rgba8 p0_end_color ,Vec3 p1 , Rgba8 p1_start_color , Rgba8 p1_end_color ,float duration ,eDebugRenderMode mode );
void DebugAddWorldArrow( Vec3 start , Vec3 end , Rgba8 color , float duration ,float thickness, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

//Quads
void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

//Screen Renders
void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 start_color , Rgba8 end_color , float duration );
void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 color , float duration = 0.0f );
void DebugAddScreenPoint( Vec2 pos , Rgba8 color ); // assumed size;





