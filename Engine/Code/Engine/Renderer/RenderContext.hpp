#pragma once
#include <Engine\Core\Vertex_PCU.hpp>
#include <Engine\Renderer\Camera.hpp>
#include "Engine\Core\AABB2.hpp"
#include "Engine\Core\Texture.hpp"
#include <map>
#include <string>
#include <vector>



class BitmapFont;
class Window;
class Shader;
class VertexBuffer;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
class SwapChain;
class RenderBuffer;
struct Mat44;
struct ID3D11BlendState;
class Sampler;

enum class BlendMode
{
	ALPHA,
	ADDITIVE
};

enum eBufferSlot
{
	UBO_FRAME_SLOT=0,
	UBO_CAMERA_SLOT=1,
};

//Data for frame ubo
struct  frameData_t
{
	float systemTime;
	float systemDeltaTime;

	float padding[ 2 ];
};

struct cameraData_t
{
	Mat44 projection;
	Mat44 view;
};

class RenderContext
{


	std::map<std::string,Texture*> m_LoadedTextures;
	std::map<std::string,BitmapFont*> m_LoadedFonts;

public:

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	SwapChain* m_swapChain = nullptr;
	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;
	Sampler* m_defaultSampler = nullptr;
	Texture* m_defaultColor = nullptr; //white
	VertexBuffer* m_immediateVBO = nullptr;
	ID3D11Buffer* m_lastBoundVBO = nullptr;
	RenderBuffer* m_frameUBO = nullptr;
	Texture* m_texture;

	ID3D11BlendState* m_alphaBlendState;
	ID3D11BlendState* m_additiveBlendState;
	bool m_isDrawing = false;

	
public:
	

	void Startup(Window* window);
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Draw( int numVertexes , int vertexOffset );
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray(const std::vector<Vertex_PCU> &verts);

	void DrawAABB2D(const AABB2& aabb,const Rgba8& color );
	void DrawXFlippedAABB2D( const AABB2& aabb , const Rgba8& color );
	void DrawLine( const Vec2& start, const Vec2& end, const Rgba8& color, float thickness );
	void DrawRing( const Vec2 centre, float radius, Rgba8 color, float thickness );
	void DrawDisc( const Vec2 centre , float radius , Rgba8 color );

	void BindShader( Shader* shader );
	void BindShader( std::string filename );
	void BindVertexInput( VertexBuffer* vbo );

	void BindUniformBuffer( unsigned int slot , RenderBuffer* ubo );

	Texture* CreateTextureFromFile( const char* imageFilePath);
	Texture* GetOrCreateTextureFromFile(const char* imageFilePath);

	Texture* CreateTextureFromColor( Rgba8 color );

	void BindTexture(  const Texture* texture);
	void BindSampler( const Sampler* sampler );

	void ClaerScreen(const Rgba8 clearColor);
	void BeginCamera(Camera &camera);
	void EndCamera(const Camera &camera);

	void UpdateFrameTime( float deltaTime );

	void TransformVertexArray(int numVertices, Vertex_PCU* vertices, float scale, float rotationDegrees, const Vec2& translation );
	void SetBlendMode(BlendMode blendMode);

	BitmapFont* CreateBitMapFontFromFile(std::string filePath);
	BitmapFont* GetOrCreateBitMapFontFromFile(std::string filePath);

private:


	

	void CreateBlendStates();



};