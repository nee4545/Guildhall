#include "Game/EntityDef.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

std::map<std::string , EntityDef*> EntityDef::s_entityDefs;

EntityDef::EntityDef()
{

}

void EntityDef::LoadDefinitions( char const* dataFilePath )
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( dataFilePath );

	tinyxml2::XMLElement* root = doc.RootElement();

	// Parse all Actors

	for ( tinyxml2::XMLElement* ele = root->FirstChildElement( "Actor" ); ele != nullptr; ele = ele->NextSiblingElement( "Actor" ) )
	{
		//parse actor
		std::string name = ParseXmlAttribute( *ele , "name" , "null" );
		if ( name == "null" )
		{
			//error
			g_theConsole.PrintError( "typename not specified skipping this actor" );
			continue;
		}

		EntityDef* newDef = new EntityDef();

		tinyxml2::XMLElement* physicsEle = ele->FirstChildElement( "Physics" );

		if ( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.f );
			newDef->m_height = ParseXmlAttribute( *physicsEle , "height" , 0.f );
			newDef->m_eyeHeight = ParseXmlAttribute(  *physicsEle , "eyeHeight" , 0.f );
			newDef->m_speed = ParseXmlAttribute( *physicsEle , "walkSpeed" , 0.0f );
		}

		tinyxml2::XMLElement* appereanceEle = ele->FirstChildElement( "Appearance" );

		if ( appereanceEle != nullptr )
		{
			newDef->m_spriteSize = ParseXmlAttribute( *appereanceEle , "size" , Vec2( 0.f , 0.f ) );
			std::string billboardtype = ParseXmlAttribute( *appereanceEle , "billboard" , "null" );

			if ( billboardtype != "null" )
			{
				if ( billboardtype == "CameraFacingXY" )
				{
					newDef->m_billboard = CAMERA_FACING_XY;
				}
				else if ( billboardtype == "CameraOpposedXY" )
				{
					newDef->m_billboard = CAMERA_OPPOSED_XY;
				}
				else if ( billboardtype == "CameraFacingXYZ" )
				{
					newDef->m_billboard = CAMERA_FACING_XYZ;
				}
				else if ( billboardtype == "CameraOpposedXYZ" )
				{
					newDef->m_billboard = CAMERA_OPPOSED_XYZ;
				}
			}

			std::string spritePath = ParseXmlAttribute( *appereanceEle , "spriteSheet" ,"null"  );
			IntVec2 spriteLayout = ParseXmlAttribute( *appereanceEle , "layout" , IntVec2( 0 , 0 ) );

			if ( spritePath != "null" && spriteLayout != IntVec2( 0 , 0 ) )
			{
				Texture* spriteTex = g_theRenderer->GetOrCreateTextureFromFile( spritePath.c_str() );
				newDef->m_spriteSheet = new SpriteSheet( *spriteTex , spriteLayout );
			}

			if ( newDef->m_spriteSheet != nullptr )
			{
				int index = 0;
				for (tinyxml2::XMLElement* walkEle = appereanceEle->FirstChildElement() ; walkEle!=nullptr ; walkEle = walkEle->NextSiblingElement(),index++ )
				{
					if ( walkEle == nullptr )
					{
						continue;
					}

					std::string front = ParseXmlAttribute( *walkEle , "front" , "null" );

					if ( front != "null" )
					{
						Strings nums = SplitStringOnDelimiter( front , ',' );

						newDef->m_entityAnims[ index ][ "front" ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
					}

					std::string back = ParseXmlAttribute( *walkEle , "back" , "null" );

					if ( back != "null" )
					{
						Strings nums = SplitStringOnDelimiter( back , ',' );

						newDef->m_entityAnims[ index ][ "back" ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
					}

					std::string left = ParseXmlAttribute( *walkEle , "left" , "null" );

					if ( left != "null" )
					{
						Strings nums = SplitStringOnDelimiter( left , ',' );

						newDef->m_entityAnims[ index ][ "left" ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
					}

					std::string right = ParseXmlAttribute( *walkEle , "right" , "null" );

					if ( right != "null" )
					{
						Strings nums = SplitStringOnDelimiter( right , ',' );

						newDef->m_entityAnims[ index ][ "right" ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
					}

					std::string frontleft = ParseXmlAttribute( *walkEle , "frontLeft" , "null" );

					if ( frontleft != "null" )
					{
						Strings nums = SplitStringOnDelimiter( frontleft , ',' );

						newDef->m_entityAnims[ index ][ "frontLeft" ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
					}

					std::string frontright = ParseXmlAttribute( *walkEle , "frontRight" , "null" );

					if ( frontright != "null" )
					{
						Strings nums = SplitStringOnDelimiter( frontright , ',' );

						newDef->m_entityAnims[ index ][ "frontRight" ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
					}

					std::string backleft = ParseXmlAttribute( *walkEle , "backLeft" , "null" );

					if ( backleft != "null" )
					{
						Strings nums = SplitStringOnDelimiter( backleft , ',' );

						newDef->m_entityAnims[ index ][ "backLeft" ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
					}

					std::string backright = ParseXmlAttribute( *walkEle , "backRight" , "null" );

					if ( backright != "null" )
					{
						Strings nums = SplitStringOnDelimiter( backright , ',' );

						newDef->m_entityAnims[ index ][ "backRight" ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
					}
				}
			}

			newDef->m_className = "Actor";
			newDef->m_typeName = name;

			s_entityDefs[ name ] = newDef;

		}

	}

	//Parse all Entities

	for ( tinyxml2::XMLElement* ele = root->FirstChildElement( "Entity" ); ele != nullptr; ele = ele->NextSiblingElement( "Entity" ) )
	{
		//parse entity
		std::string name = ParseXmlAttribute( *ele , "name" , "null" );

		if ( name == "null" )
		{
			//error
			g_theConsole.PrintError( "typename not specified skipping this entity" );
			continue;
		}
		
		EntityDef* newDef = new EntityDef();
		tinyxml2::XMLElement* physicsEle = ele->FirstChildElement( "Physics" );

		if ( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.0f );
			newDef->m_height = ParseXmlAttribute( *physicsEle , "height" , 0.0f );
		}
		newDef->m_className = "Entity";
		newDef->m_typeName = name;

		s_entityDefs[ name ] = newDef;
	}

	//Parse all projectiles
	for ( tinyxml2::XMLElement* ele = root->FirstChildElement( "Projectile" ); ele != nullptr; ele = ele->NextSiblingElement( "Projectile" ) )
	{
		//parse projectiles
		std::string name = ParseXmlAttribute( *ele , "name" , "null" );

		if ( name == "null" )
		{
			//error
			g_theConsole.PrintError( "typename not specified skipping this projectile" );
			continue;
		}

		EntityDef* newDef = new EntityDef();
		tinyxml2::XMLElement* physicsEle = ele->FirstChildElement( "Physics" );

		if ( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.0f );
			newDef->m_height = ParseXmlAttribute( *physicsEle , "height" , 0.0f );
			newDef->m_speed = ParseXmlAttribute( *physicsEle , "speed" , 0.f );
		}

		tinyxml2::XMLElement* gamePlayEle = ele->FirstChildElement( "Gameplay" );

		if ( gamePlayEle != nullptr )
		{
			newDef->m_damage = ParseXmlAttribute( *gamePlayEle , "damage" , IntVec2( 0 , 0 ) );
		}

		newDef->m_className = "Projectile";
		newDef->m_typeName = name;

		s_entityDefs[ name ] = newDef;

	}

	for ( tinyxml2::XMLElement* ele = root->FirstChildElement( "Portal" ); ele != nullptr;ele= ele->NextSiblingElement( "Portal" ) )
	{
		//parse portal
		std::string name = ParseXmlAttribute( *ele , "name" , "null" );

		if ( name == "null" )
		{
			//error
			g_theConsole.PrintError( "typename not specified skipping this portal" );
			continue;
		}


		EntityDef* newDef = new EntityDef();

		tinyxml2::XMLElement* physicsEle = ele->FirstChildElement( "Physics" );
		if ( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.f );
			newDef->m_height = ParseXmlAttribute( *physicsEle , "height" , 0.f );
		}

		tinyxml2::XMLElement* appereanceEle = ele->FirstChildElement( "Appearance" );

		if ( appereanceEle != nullptr )
		{
			std::string spritePath = ParseXmlAttribute( *appereanceEle , "spriteSheet" , "null" );
			IntVec2 spriteLayout = ParseXmlAttribute( *appereanceEle , "layout" , IntVec2( 0 , 0 ) );

			if ( spritePath != "null" && spriteLayout != IntVec2( 0 , 0 ) )
			{
				Texture* spriteTex = g_theRenderer->GetOrCreateTextureFromFile( spritePath.c_str() );
				newDef->m_spriteSheet = new SpriteSheet( *spriteTex , spriteLayout );
			}

			if ( newDef->m_spriteSheet != nullptr )
			{
				std::string animations = ParseXmlAttribute( *appereanceEle , "animations" , "null" );
				Strings nums = SplitStringOnDelimiter( animations , ',' );
				newDef->m_animations["animations"] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );

			}
			

			std::string texturePath = ParseXmlAttribute( *appereanceEle , "Texture" , "null" );

			if ( texturePath != "null" )
			{
				Texture* spriteTex = g_theRenderer->GetOrCreateTextureFromFile( texturePath.c_str() );
				newDef->m_texture = spriteTex;
			}

		}
		newDef->m_className = "Portal";
		newDef->m_typeName = name;

		s_entityDefs[ name ] = newDef;

	}


}
EntityDef const* EntityDef::GetDefinition( std::string defName )
{
	auto found = s_entityDefs.find( defName );
	
	if ( found != s_entityDefs.end() )
	{
		return s_entityDefs[ defName ];
	}
	else
	{
		ERROR_AND_DIE( "No such entityDef Exists" );
	}
}

