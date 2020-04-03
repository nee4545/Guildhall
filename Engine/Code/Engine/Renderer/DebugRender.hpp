#pragma once
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Math/Mat44.hpp"
#include <string>
#include "../Core/AABB3.hpp"

class RenderContext;
class Clock;
class Timer;
class Camera;
class Texture;
class GPUMesh;

struct D3D11_RASTERIZER_DESC;

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
	OBJECT_BASIS,
	OBJECT_TEXT_WORLD,
	OBJECT_TEXT_SCREEN,
	OBJECT_WIRE_SPHERE,
	OBJECT_WIRE_BOX,

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

class DebugWireSphere :public DebugRenderObject
{
public:
	DebugWireSphere( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 pos , float size , Rgba8 color , float duration , bool isBillboard = false );
	DebugWireSphere( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , bool isBillboard = false );
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

class DebugBasis : public DebugRenderObject
{
public:
	DebugBasis( eDebugRenderMode mode , eDebugRenderSpace space , Mat44 basis , Rgba8 color , float duration );
	DebugBasis( eDebugRenderMode mode , eDebugRenderSpace space , Mat44 basis , Rgba8 startColor, Rgba8 endColor , float duration );

	Mat44 m_basis;
};

class DebugScreenText : public DebugRenderObject
{
public:
	DebugScreenText( eDebugRenderMode mode , eDebugRenderSpace space ,float size, Rgba8 color ,float duration, Vec2 pivot , Vec4 offset, std::string text );
	DebugScreenText( eDebugRenderMode mode , eDebugRenderSpace space ,float size, Rgba8 startColor, Rgba8 endColor ,float duration, Vec2 pivot , Vec4 offset, std::string text );
	Vec4 m_offset;
	Vec2 m_pivot;
	std::string m_text;
	float m_size;
};

class DebugWorldText : public DebugRenderObject
{
public:
	DebugWorldText( eDebugRenderMode mode , eDebugRenderSpace space , float size , Rgba8 color , Mat44 basis , float duration , Vec2 pivot , std::string text, bool isBillboard = false );
	DebugWorldText( eDebugRenderMode mode , eDebugRenderSpace space , float size , Rgba8 startColor, Rgba8 endColor , Mat44 basis , float duration , Vec2 pivot , std::string text , bool isBillboard = false );

	Mat44 m_basis;
	Vec2 m_pivot;
	std::string m_text;
	float m_size;
};

class DebugWorldWireBox : public DebugRenderObject
{
public:
	DebugWorldWireBox( eDebugRenderMode mode , eDebugRenderSpace space , AABB3 box , Rgba8 startColor , Rgba8 endColor , float duration , bool isBillboard = false );
	DebugWorldWireBox( eDebugRenderMode mode , eDebugRenderSpace space , AABB3 box , Rgba8 color , float duration , bool isBillboard = false );
	//DebugWorldWireBox( eDebugRenderMode mode , eDebugRenderSpace space , OBB3 box , Rgba8 startColor , Rgba8 endColor , Mat44 basis , float duration , Vec2 pivot , std::string text , bool isBillboard = false );
	//DebugWorldWireBox( eDebugRenderMode mode , eDebugRenderSpace space , OBB3 box , Rgba8 color , Mat44 basis , float duration , Vec2 pivot , std::string text , bool isBillboard = false );

	AABB3 m_box;
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

	void EnableDebugRendering();
	void DisableDebugRendering();
	void ClearDebugRendering();

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
	bool enableRendring = true;
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

void DebugAddWorldWireBounds( AABB3 bounds , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

void DebugAddWorldBasis( Mat44 basis , Rgba8 start_tint , Rgba8 end_tint , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldBasis( Mat44 basis ,Rgba8 color, float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

void DebugAddWorldText( Mat44 basis , Vec2 pivot ,float size, Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode , char const* text );
void DebugAddWorldTextf( Mat44 basis , Vec2 pivot ,float size, Rgba8 color , float duration , eDebugRenderMode mode , char const* text , ... );
void DebugAddWorldTextf( Mat44 basis , Vec2 pivot ,float size, Rgba8 color , char const* text , ... );

void DebugAddWorldBillboardText( Vec3 origin , Vec2 pivot ,float size, Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode , char const* text );
void DebugAddWorldBillboardTextf( Vec3 origin , Vec2 pivot ,float size, Rgba8 color , float duration , eDebugRenderMode mode , char const* format , ... );
void DebugAddWorldBillboardTextf( Vec3 origin , Vec2 pivot , float size, Rgba8 color , char const* format , ... );



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

void DebugAddScreenText( Vec4 ratioOffset , Vec2 pivot , float size , Rgba8 start_color , Rgba8 end_color , float duration , char const* text );
void DebugAddScreenText( Vec4 ratioOffset , Vec2 pivot , float size , Rgba8 color , float duration , char const* text );
void DebugAddScreenTextf( Vec4 ratioOffset , Vec2 pivot , float size , Rgba8 start_color , Rgba8 end_color , float duration , char const* format , ... );
void DebugAddScreenTextf( Vec4 ratioOffset , Vec2 pivot , float size , Rgba8 color , float duration , char const* format,... );









