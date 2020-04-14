#pragma once
#include <Engine\Core\Vertex_PCU.hpp>
#include <Engine\Renderer\Camera.hpp>
#include "Engine\Core\Texture.hpp"
#include "Engine\Math\Mat44.hpp"
#include <map>
#include <string>
#include <vector>

constexpr int MAX_LIGHTS= 1;


class BitmapFont;
class Window;
class Shader;
class VertexBuffer;
class Polygon2D;
class GPUMesh;
class IndexBuffer;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
class SwapChain;
class RenderBuffer;
struct Mat44;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
class Sampler;
struct IDXGIDebug;
struct D3D11_RASTERIZER_DESC;
enum D3D11_FILL_MODE: int;
enum D3D11_CULL_MODE: int;

enum eCompareOp            // A04
{
	COMPARE_NEVER ,       // false,      "never"
	COMPARE_ALWAYS ,      // true,       "always"
	COMPARE_EQUAL ,       // ==,         "equal"
	COMPARE_NOTEQUAL ,    // !=,         "not"
	COMPARE_LESS ,        // <           "less"
	COMPARE_LEQUAL ,      // <=          "lequal"
	COMPARE_GREATER ,     // >           "greater"
	COMPARE_GEQUAL ,      // >=          "gequal"
};

enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQE,
};

enum eBufferSlot
{
	UBO_FRAME_SLOT=0,
	UBO_CAMERA_SLOT=1,
};

enum eTextureSlot
{
	TEXTURE_SLOT_DIFFUSE = 0,
	TEXTURE_SLOT_NORMAL = 1
};

enum eAttenuations
{
	ATTENUATION_LINEAR,
	ATTENUATION_QUADRATIC,
	ATTENUATION_CONSTANT,
};

struct modelData_t
{
	Mat44 modal;
};

//Data for frame ubo
struct  frameData_t
{
	float systemTime;
	float systemDeltaTime;

	float padding;
	float INVERSE_GAMMA		= 1.f/2.2f;
};

struct light_t
{
	Vec3 position;
	float pad00;

	Vec3 color;
	float intensity;

	Vec3 attenuation = Vec3(0.f,0.f,1.f);
	float dotInnerAngle;

	Vec3 specularAttunation = Vec3(1.f,0.f,0.f);
	float dotOuterAngle;
};

struct lights_data
{
	Vec4 ambientLight;
	light_t light[MAX_LIGHTS];

	// all 0 to 1 and help influence the lighting equation
   float diffuseFactor=1.f;   // default: 1  - scales diffuse lighting in equation (lower values make an object absorb light
   float specularFactor=1.f;  // default: 1  - scales specular lighting in the equation (higher values create a "shinier" surface)
   float specularPower=32.f;   // default: 32 - controls the specular falloff (higher values create a "smoother" looking surface)
   float emmissiveFactor=1.f;  // default: 1  - controls how much the emissive texture is added into the final equation
};

struct cameraData_t
{
	Mat44 projection;
	Mat44 view;

	Vec3 cameraPosition;
	float padding;
};

class RenderContext
{


	std::map<std::string,Texture*> m_LoadedTextures;
	std::map<std::string,BitmapFont*> m_LoadedFonts;
	std::map<std::string , Shader*> m_loadedShaders;

public:

	//debug
	void* m_debugModule = nullptr;
	IDXGIDebug* m_debug = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	SwapChain* m_swapChain = nullptr;
	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;
	Sampler* m_defaultSampler = nullptr;
	Texture* m_defaultColor = nullptr; //white
	VertexBuffer* m_immediateVBO = nullptr;

	ID3D11Buffer* m_lastBoundVBO = nullptr;
	//ID3D11Buffer* m_lastBoundIBO = nullptr;
	RenderBuffer* m_frameUBO = nullptr;
	RenderBuffer* m_modelUBO = nullptr;
	RenderBuffer* m_lightUBO = nullptr;

	Texture* m_texture;
	ID3D11BlendState* m_alphaBlendState;
	ID3D11BlendState* m_additiveBlendState;
	ID3D11BlendState* m_opaqueBlendState;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11RasterizerState* m_rasterState = nullptr;
	bool m_isDrawing = false;
	lights_data m_lights;

public:
	

	void Startup(Window* window);
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Draw( int numVertexes , int vertexOffset, buffer_attribute_t* layout = Vertex_PCU::LAYOUT );
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray(const std::vector<Vertex_PCU> &verts);
	void DrawVertexArray(int numVertexes, VertexBuffer* vertices );
	void DrawIndexed( unsigned int indexCount , unsigned int startIndex , unsigned int indexStride , buffer_attribute_t* layout = Vertex_PCU::LAYOUT);

	void DrawAABB2D(const AABB2& aabb,const Rgba8& color );
	void DrawXFlippedAABB2D( const AABB2& aabb , const Rgba8& color );
	void DrawLine( const Vec2& start, const Vec2& end, const Rgba8& color, float thickness );
	void DrawArrow2D( Vec2& start , Vec2& end , Rgba8 lineColor , Rgba8 arrowColor , float lineThickness );
	void DrawRing( const Vec2 centre, float radius, Rgba8 color, float thickness );
	void DrawDisc( const Vec2 centre , float radius , Rgba8 color );
	void DrawPolygonUnfilled(const Polygon2D& polygon, const Rgba8& color, float thickness );
	void DrawPolygonFilled(const Polygon2D& polygon, const Rgba8& color);
	void DrawMesh( GPUMesh* mesh );

	void BindShader( Shader* shader );
	void BindShader( std::string filename );
	void BindVertexBuffer( VertexBuffer* vbo );
	void BindIndexBuffer( IndexBuffer* ibo );
	void BindDepthStencil( Texture* dsv );
	void SetDepthTest(eCompareOp comparision = COMPARE_LESS,bool writePass = true);
	void ClearDepth( Texture* dsTarget , float depth );

	Shader* GetOrCreateShader( char const* filename );
	void SetModalMatrix( Mat44 mat );

	void BindUniformBuffer( unsigned int slot , RenderBuffer* ubo );
	void CreateRasterState( D3D11_FILL_MODE fillmode , D3D11_CULL_MODE cullmode , bool frontCounterClockWise = true );
	void CreateRasterState( D3D11_RASTERIZER_DESC desc );
	

	Texture* CreateTextureFromFile( const char* imageFilePath);
	Texture* GetOrCreateTextureFromFile(const char* imageFilePath);

	Texture* CreateTextureFromColor( Rgba8 color );

	void BindTexture(  const Texture* texture , eTextureSlot textureType = eTextureSlot::TEXTURE_SLOT_DIFFUSE );
	void BindSampler( const Sampler* sampler );

	void ClaerScreen(const Rgba8 clearColor);
	void BeginCamera(Camera &camera);
	void EndCamera(const Camera &camera);

	void UpdateFrameTime( float deltaTime );

	void TransformVertexArray(int numVertices, Vertex_PCU* vertices, float scale, float rotationDegrees, const Vec2& translation );
	void SetBlendMode(BlendMode blendMode);

	BitmapFont* CreateBitMapFontFromFile(std::string filePath);
	BitmapFont* GetOrCreateBitMapFontFromFile(std::string filePath);

	void SetAmbientColor( Rgba8 color );
	void SetAmbientIntensity( float intensity );
	void SetAmbientLight( Rgba8 color , float intensity );
	void SetSpecularConstants( float specularPower , float specularFactor );
	void EnableLight( unsigned int idx , light_t const& lightInfo );
	void SetAttenuationFactors( eAttenuations factor, unsigned int lightId );
	void SetSpecularFactor( float factor );
	void SetSpecularPower( float power );
	void SetSpecularAttenuation( Vec3 attenuation , unsigned int lightId );
	void SetDiffuseAttenuation( Vec3 attenuation , unsigned int lightId );

private:

	void CreateDebugModule();
	void DestroyDebugModule();
	void ReportLiveObjects();
	void CreateBlendStates();
};