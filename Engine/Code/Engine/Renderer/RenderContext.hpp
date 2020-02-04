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
class SwapChain;

enum class BlendMode
{
	ALPHA,
	ADDITIVE
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
	VertexBuffer* m_immediateVBO = nullptr;

	
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
	void BindVertexInput( VertexBuffer* vbo );

	Texture* CreateTextureFromFile( const char* imageFilePath);
	Texture* GetOrCreateTextureFromFile(const char* imageFilePath);
	void BindTexture( const Texture* texture);

	void ClaerScreen(const Rgba8 clearColor);
	void BeginCamera(const Camera &camera);
	void EndCamera(const Camera &camera);

	void TransformVertexArray(int numVertices, Vertex_PCU* vertices, float scale, float rotationDegrees, const Vec2& translation );
	void SetBlendMode(BlendMode blendMode);

	BitmapFont* CreateBitMapFontFromFile(std::string filePath);
	BitmapFont* GetOrCreateBitMapFontFromFile(std::string filePath);
};