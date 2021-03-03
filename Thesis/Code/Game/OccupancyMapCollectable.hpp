#pragma once
#include "Engine/Core/Vertex_PCU.hpp"

class SpriteAnimDefTex;
class Game;

class OccMapCollectable
{
public:
OccMapCollectable(Game* game,Vec2 position);
~OccMapCollectable();

void LoadAnimations();
void Update(float deltaSeconds);
void Render();

Vertex_PCU m_vertices[ 6 ];
SpriteAnimDefTex* m_anims = nullptr;
float m_time = 0.f;
Vec2 m_position;
Game* m_game = nullptr;
bool m_isGarbage = false;

};