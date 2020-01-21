#pragma once
#include "Engine/Core/AABB2.hpp"

class HUD
{
public:
	AABB2 m_HUDBox;
	AABB2 m_dinoBox;
	AABB2 m_dinoCostBox;
	AABB2 m_spikeBox;
	AABB2 m_spikeCostBox;
	AABB2 m_heroBox;
	AABB2 m_heroCostBox;
	AABB2 m_stoneBox;
	AABB2 m_stoneCostBox;
	AABB2 m_dinoSprite;
	AABB2 m_spikeSprite;
	AABB2 m_heroSprite;
	AABB2 m_stoneSprite;
	AABB2 m_selectedBox;

	HUD();
	void Render();
	void RenderSelectedUnit( int unit );

};