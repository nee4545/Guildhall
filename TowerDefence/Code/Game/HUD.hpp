#pragma once
#include "Engine/Core/AABB2.hpp"

class Map;
class BitmapFont;

class HUD
{
public:
	AABB2 m_HUDBox;
	AABB2 m_dinoBox;
	AABB2 m_dinoCostBox;
	AABB2 m_dinoUpgradeBox;

	AABB2 m_spikeBox;
	AABB2 m_spikeCostBox;
	AABB2 m_spikeUpgradeBox;

	AABB2 m_heroBox;
	AABB2 m_heroCostBox;
	AABB2 m_heroUpgradeBox;

	AABB2 m_stoneBox;
	AABB2 m_stoneCostBox;
	AABB2 m_dinoSprite;
	AABB2 m_spikeSprite;
	AABB2 m_heroSprite;
	AABB2 m_stoneSprite;
	AABB2 m_selectedBox;

	AABB2 m_playerCoinBox;
	AABB2 m_enemyCoinBox;

	AABB2 m_playerCoins;
	AABB2 m_enemyCoins;

	Map* m_map = nullptr;
	BitmapFont* m_font = nullptr;

	HUD();
	void Render();
	void RenderSelectedUnit( int unit );

};