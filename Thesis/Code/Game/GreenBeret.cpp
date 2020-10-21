#include "Game/GreenBeret.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/PotentialFields.hpp"


GreenBeret::GreenBeret(Game* game)
{
	m_game = game;
	LoadCrouchAnimations();
	LoadWalkAnimations();

	m_vertices[ 0 ] = Vertex_PCU( Vec3( -aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_minUV );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_maxUV );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );
}

GreenBeret::~GreenBeret()
{
	
}

void GreenBeret::Update( float deltaseconds )
{
	m_time += deltaseconds;
	

	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_nextMovePosition = m_game->m_mousePosition;
		m_moveSet = true;
	}

	if ( m_moveSet )
	{
		if ( ( m_position - m_nextMovePosition ).GetLengthSquared() >= 0.1f )
		{
			Vec2 dirVec = ( m_nextMovePosition - m_position ).GetNormalized();

			if ( IsPointInsideDisc2D( m_position , m_game->m_potentialField->GetCentre() , m_game->m_potentialField->GetBoundingRadius() ) )
			{

				if ( !potentialFieldQueryDone )
				{
					m_game->m_potentialField->PotentialFieldQueryForEntityEndPosition( m_nextMovePosition , shoudlGetFlippedPotentialFieldValues , shoudCachePosition , m_cachedNextPosition );

					if ( shoudCachePosition )
					{
						Vec2 temp = m_nextMovePosition;
						m_nextMovePosition = m_cachedNextPosition;
						m_cachedNextPosition = temp;
					}

					potentialFieldQueryDone = true;

				}

				float degree = m_game->m_potentialField->GetPotentialFieldForPosition( m_position, shoudlGetFlippedPotentialFieldValues );
				if ( degree >= 0.f )
				{
					dirVec = Vec2::MakeFromPolarDegrees( degree );
					dirVec = dirVec.GetNormalized();
				}
			}

			m_position += dirVec * 7.f * deltaseconds;
			SetTexture();

		
		}
		else if ( shoudCachePosition )
		{
			m_nextMovePosition = m_cachedNextPosition;
			shoudCachePosition = false;
		}
		else
		{
			m_moveSet = false;
			potentialFieldQueryDone = false;
			shoudCachePosition = false;
			shoudlGetFlippedPotentialFieldValues = false;

		}
		
	}
}

void GreenBeret::Render()
{
	
	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformVertexArray( 6 , vertCopy , 4.5f , m_orientationDegrees , m_position );

	g_theRenderer->BindTexture( m_currentTex );
	g_theRenderer->DrawVertexArray( 6, vertCopy );
	g_theRenderer->BindTexture( nullptr );
	


}

void GreenBeret::Die()
{
	m_isGarbage = true;
}

void GreenBeret::LoadAnimations()
{
	
}

void GreenBeret::LoadCrouchAnimations()
{
	std::vector<Texture*> crouch1;
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0007.png" );

	crouch1.push_back( a1 );
	crouch1.push_back( a2 );
	crouch1.push_back( a3 );
	crouch1.push_back( a4 );
	crouch1.push_back( a5 );
	crouch1.push_back( a6 );
	crouch1.push_back( a7 );
	crouch1.push_back( a8 );

	m_crouchR = new AnimProperties();
	m_crouchR->animations = new SpriteAnimDefTex( 0 , ( int ) crouch1.size() - 1 , 1.f );
	m_crouchR->animations->m_animations = crouch1;
	m_crouchR->aspect = Vec2( 0.45f , 0.24f );

	std::vector<Texture*> crouch2;
	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0008.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0009.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0010.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0011.png" );
	Texture* b5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0012.png" );
	Texture* b6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0013.png" );
	Texture* b7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0014.png" );
	Texture* b8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0015.png" );

	crouch2.push_back( b1 );
	crouch2.push_back( b2 );
	crouch2.push_back( b3 );
	crouch2.push_back( b4 );
	crouch2.push_back( b5 );
	crouch2.push_back( b6 );
	crouch2.push_back( b7 );
	crouch2.push_back( b8 );

	m_crouchR1 = new AnimProperties();
	m_crouchR1->animations = new SpriteAnimDefTex( 0 , ( int ) crouch2.size() - 1 , 1.f );
	m_crouchR1->animations->m_animations = crouch2;
	m_crouchR1->aspect = Vec2( 0.46f , 0.29f );

	std::vector<Texture*> crouch3;
	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0016.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0017.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0018.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0019.png" );
	Texture* c5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0020.png" );
	Texture* c6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0021.png" );
	Texture* c7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0022.png" );
	Texture* c8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0023.png" );

	crouch3.push_back( c1 );
	crouch3.push_back( c2 );
	crouch3.push_back( c3 );
	crouch3.push_back( c4 );
	crouch3.push_back( c5 );
	crouch3.push_back( c6 );
	crouch3.push_back( c7 );
	crouch3.push_back( c8 );

	m_crouchR2 = new AnimProperties();
	m_crouchR2->animations = new SpriteAnimDefTex( 0 , ( int ) crouch3.size() - 1 , 1.f );
	m_crouchR2->animations->m_animations = crouch3;
	m_crouchR2->aspect = Vec2( 0.43f , 0.33f );

	std::vector<Texture*> crouch4;
	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0024.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0025.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0026.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0027.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0028.png" );
	Texture* d6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0029.png" );
	Texture* d7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0030.png" );
	Texture* d8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0031.png" );

	crouch4.push_back( d1 );
	crouch4.push_back( d2 );
	crouch4.push_back( d3 );
	crouch4.push_back( d4 );
	crouch4.push_back( d5 );
	crouch4.push_back( d6 );
	crouch4.push_back( d7 );
	crouch4.push_back( d8 );

	m_crouchR3 = new AnimProperties();
	m_crouchR3->animations = new SpriteAnimDefTex( 0 , ( int ) crouch4.size() - 1 , 1.f );
	m_crouchR3->animations->m_animations = crouch4;
	m_crouchR3->aspect = Vec2( 0.37f , 0.35f );


	std::vector<Texture*> crouch5;
	Texture* e1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0032.png" );
	Texture* e2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0033.png" );
	Texture* e3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0034.png" );
	Texture* e4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0035.png" );
	Texture* e5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0036.png" );
	Texture* e6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0037.png" );
	Texture* e7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0038.png" );
	Texture* e8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0039.png" );

	crouch5.push_back( e1 );
	crouch5.push_back( e2 );
	crouch5.push_back( e3 );
	crouch5.push_back( e4 );
	crouch5.push_back( e5 );
	crouch5.push_back( e6 );
	crouch5.push_back( e7 );
	crouch5.push_back( e8 );

	m_crouchB = new AnimProperties();
	m_crouchB->animations = new SpriteAnimDefTex( 0 , ( int ) crouch5.size() - 1 , 1.f );
	m_crouchB->animations->m_animations = crouch5;
	m_crouchB->aspect = Vec2( 0.28f , 0.34f );


	std::vector<Texture*> crouch6;
	Texture* f1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0040.png" );
	Texture* f2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0041.png" );
	Texture* f3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0042.png" );
	Texture* f4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0043.png" );
	Texture* f5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0044.png" );
	Texture* f6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0045.png" );
	Texture* f7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0046.png" );
	Texture* f8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0047.png" );

	crouch6.push_back( f1 );
	crouch6.push_back( f2 );
	crouch6.push_back( f3 );
	crouch6.push_back( f4 );
	crouch6.push_back( f5 );
	crouch6.push_back( f6 );
	crouch6.push_back( f7 );
	crouch6.push_back( f8 );

	m_crouchT = new AnimProperties();
	m_crouchT->animations = new SpriteAnimDefTex( 0 , ( int ) crouch6.size() - 1 , 1.f );
	m_crouchT->animations->m_animations = crouch6;
	m_crouchT->aspect = Vec2( 0.28f , 0.38f );


	std::vector<Texture*> crouch7;
	Texture* g1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0048.png" );
	Texture* g2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0049.png" );
	Texture* g3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0050.png" );
	Texture* g4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0051.png" );
	Texture* g5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0052.png" );
	Texture* g6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0053.png" );
	Texture* g7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0054.png" );
	Texture* g8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0055.png" );

	crouch7.push_back( g1 );
	crouch7.push_back( g2 );
	crouch7.push_back( g3 );
	crouch7.push_back( g4 );
	crouch7.push_back( g5 );
	crouch7.push_back( g6 );
	crouch7.push_back( g7 );
	crouch7.push_back( g8 );

	m_crouchT1 = new AnimProperties();
	m_crouchT1->animations = new SpriteAnimDefTex( 0 , ( int ) crouch7.size() - 1 , 1.f );
	m_crouchT1->animations->m_animations = crouch7;
	m_crouchT1->aspect = Vec2( 0.36f , 0.38f );

	std::vector<Texture*> crouch8;
	Texture* h1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0056.png" );
	Texture* h2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0057.png" );
	Texture* h3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0058.png" );
	Texture* h4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0059.png" );
	Texture* h5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0060.png" );
	Texture* h6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0061.png" );
	Texture* h7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0062.png" );
	Texture* h8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0063.png" );

	crouch8.push_back( h1 );
	crouch8.push_back( h2 );
	crouch8.push_back( h3 );
	crouch8.push_back( h4 );
	crouch8.push_back( h5 );
	crouch8.push_back( h6 );
	crouch8.push_back( h7 );
	crouch8.push_back( h8 );

	m_crouchT2 = new AnimProperties();
	m_crouchT2->animations = new SpriteAnimDefTex( 0 , ( int ) crouch8.size() - 1 , 1.f );
	m_crouchT2->animations->m_animations = crouch8;
	m_crouchT2->aspect = Vec2( 0.42f , 0.36f );


	std::vector<Texture*> crouch9;
	Texture* i1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0064.png" );
	Texture* i2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0065.png" );
	Texture* i3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0066.png" );
	Texture* i4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0067.png" );
	Texture* i5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0068.png" );
	Texture* i6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0069.png" );
	Texture* i7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0070.png" );
	Texture* i8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Crouch/CDAR0071.png" );

	crouch9.push_back( i1 );
	crouch9.push_back( i2 );
	crouch9.push_back( i3 );
	crouch9.push_back( i4 );
	crouch9.push_back( i5 );
	crouch9.push_back( i6 );
	crouch9.push_back( i7 );
	crouch9.push_back( i8 );

	m_crouchT3 = new AnimProperties();
	m_crouchT3->animations = new SpriteAnimDefTex( 0 , ( int ) crouch9.size() - 1 , 1.f );
	m_crouchT3->animations->m_animations = crouch9;
	m_crouchT3->aspect = Vec2( 0.46f , 0.30f );

}

void GreenBeret::LoadWalkAnimations()
{
	std::vector<Texture*> walk1;
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0007.png" );

	walk1.push_back( a1 );
	walk1.push_back( a2 );
	walk1.push_back( a3 );
	walk1.push_back( a4 );
	walk1.push_back( a5 );
	walk1.push_back( a6 );
	walk1.push_back( a7 );
	walk1.push_back( a8 );

	m_walkR = new AnimProperties();
	m_walkR->animations = new SpriteAnimDefTex( 0 , ( int ) walk1.size() - 1 , 1.f );
	m_walkR->animations->m_animations = walk1;
	m_walkR->aspect = Vec2( 0.32f , 0.40f );


	std::vector<Texture*> walk2;
	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0008.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0009.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0010.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0011.png" );
	Texture* b5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0012.png" );
	Texture* b6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0013.png" );
	Texture* b7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0014.png" );
	Texture* b8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0015.png" );

	walk2.push_back( b1 );
	walk2.push_back( b2 );
	walk2.push_back( b3 );
	walk2.push_back( b4 );
	walk2.push_back( b5 );
	walk2.push_back( b6 );
	walk2.push_back( b7 );
	walk2.push_back( b8 );

	m_walkR1 = new AnimProperties();
	m_walkR1->animations = new SpriteAnimDefTex( 0 , ( int ) walk2.size() - 1 , 1.f );
	m_walkR1->animations->m_animations = walk2;
	m_walkR1->aspect = Vec2( 0.27f , 0.40f );


	std::vector<Texture*> walk3;
	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0016.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0017.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0018.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0010.png" );
	Texture* c5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0020.png" );
	Texture* c6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0021.png" );
	Texture* c7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0022.png" );
	Texture* c8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0023.png" );

	walk3.push_back( c1 );
	walk3.push_back( c2 );
	walk3.push_back( c3 );
	walk3.push_back( c4 );
	walk3.push_back( c5 );
	walk3.push_back( c6 );
	walk3.push_back( c7 );
	walk3.push_back( c8 );

	m_walkB = new AnimProperties();
	m_walkB->animations = new SpriteAnimDefTex( 0 , ( int ) walk3.size() - 1 , 1.f );
	m_walkB->animations->m_animations = walk3;
	m_walkB->aspect = Vec2( 0.21f , 0.40f );


	std::vector<Texture*> walk4;
	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0024.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0025.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0026.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0027.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0028.png" );
	Texture* d6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0029.png" );
	Texture* d7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0030.png" );
	Texture* d8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0031.png" );

	walk4.push_back( d1 );
	walk4.push_back( d2 );
	walk4.push_back( d3 );
	walk4.push_back( d4 );
	walk4.push_back( d5 );
	walk4.push_back( d6 );
	walk4.push_back( d7 );
	walk4.push_back( d8 );

	m_walkT = new AnimProperties();
	m_walkT->animations = new SpriteAnimDefTex( 0 , ( int ) walk4.size() - 1 , 1.f );
	m_walkT->animations->m_animations = walk4;
	m_walkT->aspect = Vec2( 0.21f , 0.48f );


	std::vector<Texture*> walk5;
	Texture* e1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0032.png" );
	Texture* e2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0033.png" );
	Texture* e3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0034.png" );
	Texture* e4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0035.png" );
	Texture* e5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0036.png" );
	Texture* e6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0037.png" );
	Texture* e7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0038.png" );
	Texture* e8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Commandos/GreenBeret/Walk/CDCR0039.png" );

	walk5.push_back( e1 );
	walk5.push_back( e2 );
	walk5.push_back( e3 );
	walk5.push_back( e4 );
	walk5.push_back( e5 );
	walk5.push_back( e6 );
	walk5.push_back( e7 );
	walk5.push_back( e8 );

	m_walkT1 = new AnimProperties();
	m_walkT1->animations = new SpriteAnimDefTex( 0 , ( int ) walk5.size() - 1 , 1.f );
	m_walkT1->animations->m_animations = walk5;
	m_walkT1->aspect = Vec2( 0.27f , 0.44f );


}

void GreenBeret::SetTexture()
{
	float angleDegrees = (m_nextMovePosition - m_position).GetNormalized().GetAngleDegrees();
	g_theConsole.PrintString( Rgba8() , std::to_string( angleDegrees ) );

	if ( m_state == CROUCH )
	{
		if ( angleDegrees <= 10.f && angleDegrees >= 0.f )
		{
			m_currentTex = m_crouchR->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR->aspect;
			ResetVertices();
		}

		if ( angleDegrees >= -20.f && angleDegrees < 0.f )
		{
			m_currentTex = m_crouchR1->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR1->aspect;
			ResetVertices();
		}

		if ( angleDegrees >= -40.f && angleDegrees < -20.f )
		{
			m_currentTex = m_crouchR2->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR2->aspect;
			ResetVertices();
		}

		if ( angleDegrees >= -60.f && angleDegrees < -40.f )
		{
			m_currentTex = m_crouchR3->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR3->aspect;
			ResetVertices();
		}

		if ( angleDegrees >= -90.f && angleDegrees < -60.f )
		{
			m_currentTex = m_crouchB->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchB->aspect;
			ResetVertices();
		}

		if ( angleDegrees <= 30.f && angleDegrees > 10.f )
		{
			m_currentTex = m_crouchT3->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchT3->aspect;
			ResetVertices();
		}

		if ( angleDegrees <= 50.f && angleDegrees > 30.f )
		{
			m_currentTex = m_crouchT2->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchT2->aspect;
			ResetVertices();
		}

		if ( angleDegrees <= 70.f && angleDegrees > 50.f )
		{
			m_currentTex = m_crouchT1->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchT1->aspect;
			ResetVertices();
		}

		if ( angleDegrees <= 100.f && angleDegrees > 70.f )
		{
			m_currentTex = m_crouchT->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchT->aspect;
			ResetVertices();
		}

		if ( angleDegrees <= 130.f && angleDegrees > 100.f )
		{
			m_currentTex = m_crouchT1->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchT1->aspect;
			ResetAndFlipVertices();
		}

		if ( angleDegrees <= 150.f && angleDegrees > 130.f )
		{
			m_currentTex = m_crouchT2->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchT2->aspect;
			ResetAndFlipVertices();
		}

		if ( angleDegrees <= 180.f && angleDegrees > 150.f )
		{
			m_currentTex = m_crouchT3->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchT3->aspect;
			ResetAndFlipVertices();
		}

		if ( angleDegrees <= 180.f && angleDegrees > 165.f )
		{
			m_currentTex = m_crouchR->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR->aspect;
			ResetAndFlipVertices();
		}

		if ( angleDegrees >= -180.f && angleDegrees < -165.f )
		{
			m_currentTex = m_crouchR->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR->aspect;
			ResetAndFlipVertices();
		}

		if ( angleDegrees >= -165.f && angleDegrees < -140.f )
		{
			m_currentTex = m_crouchR1->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR1->aspect;
			ResetAndFlipVertices();
		}

		if ( angleDegrees >= -140.f && angleDegrees < -125.f )
		{
			m_currentTex = m_crouchR2->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR2->aspect;
			ResetAndFlipVertices();
		}

		if ( angleDegrees >= -125.f && angleDegrees < -110.f )
		{
			m_currentTex = m_crouchR3->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchR3->aspect;
			ResetAndFlipVertices();
		}

		if ( angleDegrees >= -110.f && angleDegrees < -91.f )
		{
			m_currentTex = m_crouchB->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_crouchB->aspect;
			ResetVertices();
		}
	}

	if ( m_state == WALK )
	{
		if ( angleDegrees <= 15.f && angleDegrees >= -10.f )
		{
			m_currentTex = m_walkR->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkR->aspect;
			ResetVertices();
		}

		else if ( angleDegrees >= -75.f && angleDegrees < -15.f )
		{
			m_currentTex = m_walkR1->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkR1->aspect;
			ResetVertices();
		}

		else if ( angleDegrees >= -115.f && angleDegrees < -75.f )
		{
			m_currentTex = m_walkB->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkB->aspect;
			ResetVertices();
		}

		else if ( angleDegrees >= -155.f && angleDegrees < -115.f )
		{
			m_currentTex = m_walkR1->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkR1->aspect;
			ResetAndFlipVertices();
		}

		else if ( angleDegrees >= -180.f && angleDegrees < -155.f )
		{
			m_currentTex = m_walkR->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkR->aspect;
			ResetAndFlipVertices();
		}

		else if(angleDegrees >= 155.f &&  angleDegrees <180.f   )
		{
			m_currentTex = m_walkR->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkR->aspect;
			ResetAndFlipVertices();
		}

		else if ( angleDegrees < 155.f && angleDegrees >= 110.f )
		{
			m_currentTex = m_walkT1->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkT1->aspect;
			ResetAndFlipVertices();
		}

		else if ( angleDegrees > 20.f && angleDegrees <= 75.f )
		{
			m_currentTex = m_walkT1->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkT1->aspect;
			ResetVertices();
		}

		else 
		{
			m_currentTex = m_walkT->animations->GetSpriteTextureAtTime( m_time );
			aspect = m_walkT->aspect;
			ResetVertices();
		}
	}

}

void GreenBeret::ResetVertices()
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_minUV );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_maxUV );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );
}

void GreenBeret::ResetAndFlipVertices()
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2(m_maxUV.x,m_minUV.y) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_minUV );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_maxUV );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_minUV );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2(m_minUV.x,m_maxUV.y) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_maxUV );
}
