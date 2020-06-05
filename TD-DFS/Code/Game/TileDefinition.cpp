
#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Texture.hpp"
#include "Game/GameCommon.hpp"

std::vector<TileDefinition> TileDefinition:: s_definitions;


TileDefinition::TileDefinition( SpriteDefinition sprite, Rgba8 tint, bool solid ):m_sprite(sprite),tintColor(tint),isSolid(solid)
{

}

void TileDefinition::CreateTileDefinitions()
{
	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile("Data/Images/Terrain_8x8.png");
	SpriteSheet* spriteSheet = new SpriteSheet(*texture,IntVec2(8,8));

	const SpriteDefinition& grass= spriteSheet->GetSpriteDefinition(15);
	s_definitions.push_back(TileDefinition(grass,Rgba8(255,255,255,255),false));

	const SpriteDefinition& stone= spriteSheet->GetSpriteDefinition( 31 );
	s_definitions.push_back( TileDefinition( stone, Rgba8( 255, 255, 255, 255 ), true ) );

	const SpriteDefinition& mud = spriteSheet->GetSpriteDefinition(24);
	s_definitions.push_back(TileDefinition(mud,Rgba8(255,255,255,255),false));

	const SpriteDefinition& exit = spriteSheet->GetSpriteDefinition( 57 );
	s_definitions.push_back( TileDefinition( exit, Rgba8( 255, 255, 255, 255 ), false ) );

	const SpriteDefinition& startTile = spriteSheet->GetSpriteDefinition(24);
	s_definitions.push_back(TileDefinition(startTile,Rgba8(255,255,255,255),false));

	const SpriteDefinition& edgeTile = spriteSheet->GetSpriteDefinition( 32 );
	s_definitions.push_back( TileDefinition( edgeTile, Rgba8( 255, 255, 255, 255 ), true ) );
}

void TileDefinition::GetSpriteSheetUVs( Vec2 minuvs, Vec2 maxuvs )
{

}
