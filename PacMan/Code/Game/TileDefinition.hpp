#pragma once
#include<vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

class TileDefinition
{

public:
 static std::vector<TileDefinition> s_definitions;
 Rgba8 tintColor=Rgba8(255,255,255,255);
 bool isSolid=false;
 Vec2 minUVs;
 Vec2 maxUVs;
 SpriteDefinition m_sprite;

 explicit TileDefinition( SpriteDefinition sprite, Rgba8 tint , bool Solid);
 static void CreateTileDefinitions();
 void GetSpriteSheetUVs(Vec2 minuvs, Vec2 maxuvs);


};