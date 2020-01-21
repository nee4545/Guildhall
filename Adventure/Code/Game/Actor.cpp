#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"

Actor::Actor( Game* game, Vec2 position, ActorDefinition* definition ):Entity(game,position)
{
	m_physicsRadius = (float)atof(definition->physicsRadius.c_str());
	m_cosmeticRadius =(float)atof(definition->cosmeticRadius.c_str());
	m_canFly = definition->canFly;
	m_canWalk = definition->canWalk;
	m_canSwim = definition->canSwim;
	m_name = definition->name;
	m_spriteAnimations = definition->m_spriteAnims;
	


	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/KushnariovaCharacters_12x53.png" );
	SpriteSheet* spriteSheet = new SpriteSheet( *texture, IntVec2( 12, 53 ) );
	const SpriteDefinition& actorSprite= spriteSheet->GetSpriteDefinition( 55 );
	Vec2 uvMins;
	Vec2 uvMaxs;
	actorSprite.GetUVs( uvMins, uvMaxs );

	m_vertices[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), uvMins );
	m_vertices[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMaxs.x, uvMins.y ) );
	m_vertices[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMins.x, uvMaxs.y ) );

	m_vertices[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMaxs.x, uvMins.y ) );
	m_vertices[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), uvMaxs );
	m_vertices[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMins.x, uvMaxs.y ) );

	m_vertices_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), uvMins );
	m_vertices_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMaxs.x, uvMins.y ) );
	m_vertices_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMins.x, uvMaxs.y ) );

	m_vertices_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMaxs.x, uvMins.y ) );
	m_vertices_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), uvMaxs );
	m_vertices_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMins.x, uvMaxs.y ) );
}

void Actor::Update( float deltaSeconds )
{
	UpdateFromJoystick(deltaSeconds);
	m_timeElapsedInSeconds += deltaSeconds;
	
}

void Actor::Render()
{
	SpriteAnimDefinition* anims = m_spriteAnimations->GetDefinition( "Idle" );
	if ( m_orientationDegrees == 0.f )
	{
		anims = m_spriteAnimations->GetDefinition( "Idle" );
	}
	else if ( m_orientationDegrees <= 110.f && m_orientationDegrees >= 70.f )
	{
		anims = m_spriteAnimations->GetDefinition( "MoveNorth" );
	}
	else if ( m_orientationDegrees <= 180.f && m_orientationDegrees >= 90.f )
	{
		anims = m_spriteAnimations->GetDefinition( "MoveWest" );
	}
	else if ( m_orientationDegrees>0.f && m_orientationDegrees<90.f )
	{
		anims = m_spriteAnimations->GetDefinition( "MoveEast" );
	}
	
	const SpriteDefinition& currentDef = anims->GetSpriteDefAtTime( m_timeElapsedInSeconds );
	currentDef.GetUVs( m_uvMins , m_uvMaxs );
	ResetUvs();

	g_theRenderer->TransformVertexArray(6,m_vertices,1.f,0.f,m_position);
	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/KushnariovaCharacters_12x53.png" );
	g_theRenderer->BindTexture( texture );
	g_theRenderer->DrawVertexArray(6,m_vertices);
	g_theRenderer->BindTexture(nullptr);
	ResetVertices();

	
}

void Actor::Die()
{
	m_isGarbage = true;
}

void Actor::ResetVertices()
{
	for( int index=0; index<6; index++ )
	{
		m_vertices[index]=m_vertices_original[index];
	}
}

void Actor::ResetUvs()
{
	m_vertices[ 0 ].m_uvTexCoords = m_uvMins;
	m_vertices[ 1 ].m_uvTexCoords = Vec2( m_uvMaxs.x , m_uvMins.y );
	m_vertices[ 2 ].m_uvTexCoords = Vec2( m_uvMins.x , m_uvMaxs.y );
	m_vertices[ 3 ].m_uvTexCoords = Vec2( m_uvMaxs.x , m_uvMins.y );
	m_vertices[ 4 ].m_uvTexCoords = m_uvMaxs;
	m_vertices[ 5 ].m_uvTexCoords = Vec2( m_uvMins.x , m_uvMaxs.y );
}

void Actor::UpdateFromJoystick( float deltaSeconds )
{
	const XboxController& controller= g_theInput->GetXboxController( 0 );

	const AnalogJoyStick& leftStick = controller.GetLeftJoystick();

	if( !controller.IsCOnnected() )
	{
		return;
	}

	float leftStickMagnitude = leftStick.GetMagnitude();
	if( leftStickMagnitude > 0.f )
	{
		m_orientationDegrees = leftStick.GetAngleDegrees();
		MoveActor( deltaSeconds );
	}
	else
	{
		m_orientationDegrees = 0.f;
	}

}

void Actor::MoveActor( float deltaSeconds )
{
	m_position.x+=CosDegrees( m_orientationDegrees )*1.f*deltaSeconds;
	m_position.y+=SinDegrees( m_orientationDegrees )*1.f*deltaSeconds;
}

Strings Actor::GetActorDetails()
{
	Strings toReturn;
	std::string temp1 = "true";
	std::string temp2 = "false";
	
	if ( m_canFly )
	{
		toReturn.push_back( "canFly: " + temp1 );
	}
	else
	{
		toReturn.push_back( "canFly: " + temp2 );
	}

	if ( m_canSwim )
	{
		toReturn.push_back( "canSwim: " + temp1 );
	}
	else
	{
		toReturn.push_back( "canSwim: " + temp2 );
	}

	if ( m_canWalk )
	{
		toReturn.push_back( "canWalk: " + temp1 );
	}
	else
	{
		toReturn.push_back( "canWalk: " + temp2 );
	}

	toReturn.push_back( m_name );

	return toReturn;
}
