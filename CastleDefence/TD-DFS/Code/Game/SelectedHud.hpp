#pragma once

#include "Engine/Core/AABB2.hpp"
class Game;
class Texture;

class SelectedHUD
{
public:

	SelectedHUD( Game* game );

	AABB2 m_textBox;
	AABB2 m_spriteBox;
	AABB2 m_sprite;

	Game* m_game = nullptr;

	Texture* m_selectedUnitTex = nullptr;
	Texture* m_textboxTexture = nullptr;
	Texture* m_spriteBoxTexture = nullptr;
	Texture* m_spriteTex = nullptr;

	Texture* m_MeleeUpgrade0Tex = nullptr;
	Texture* m_MeleeUpgrade1Tex = nullptr;
	Texture* m_MeleeUpgrade2Tex = nullptr;
	Texture* m_MeleeUpgrade3Tex = nullptr;

	Texture* m_RangeUpgrade0Tex = nullptr;
	Texture* m_RangeUpgrade1Tex = nullptr;
	Texture* m_RangeUpgrade2Tex = nullptr;
	Texture* m_RangeUpgrade3Tex = nullptr;

	

	void UpdateTexturesBasedOnSelection();
	void LoadTextures();
	void Render();
	
};