#pragma once
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/GameCommon.hpp"
#include <string>
#include <map>

class SpriteSheet;
class SpriteAnimSet;
class SpriteAnimDefinition;
class Texture;

enum ENTITYANIMS
{
	ENTITY_WALK_ANIM =0, 
	ENTITY_ATTACK_ANIM ,
	ENTITY_PAIN_ANIM , 
	ENTITY_DEATH_ANIM ,
	
	NUM_TOTAL_ENTITY_ANIMS
};

class EntityDef
{
public:
	EntityDef( tinyxml2::XMLElement* xmlEntityDef );
	EntityDef();
	~EntityDef() {};

	static void LoadDefinitions( char const* dataFilePath );
	static EntityDef const* GetDefinition( std::string defName );
	static std::map<std::string , EntityDef*> s_entityDefs;

	std::string m_typeName;
	std::string m_className;
	SpriteSheet* m_spriteSheet = nullptr;
	std::map<std::string , SpriteAnimDefinition*> m_entityAnims[ NUM_TOTAL_ENTITY_ANIMS ];
	std::map<std::string , SpriteAnimDefinition*> m_animations;

	float m_physicsRadius = 0.3f;
	float m_height = 0.6f;
	float m_eyeHeight = 0.55f;
	Vec2 m_spriteSize = Vec2( 1.f , 1.f );
	bool m_pushesEntities = false;
	bool m_pushedByEntities = false;
	bool m_pushedByWalls = false;
	float m_mass = 1.f;
	IntVec2 m_damage;
	float m_speed = 0.f;
	eBillBoardType m_billboard = CAMERA_FACING_XY;
	Texture* m_texture = nullptr;

};


