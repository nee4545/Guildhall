#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"

class SpriteSheet;

constexpr float CLIENT_ASPECT = 16.f/9.f; // We are requesting a 2:1 aspect (square) window area
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern InputSystem* g_theInput;
extern SpriteSheet* g_tileSpriteSheet;


constexpr float WORLD_SIZE_X = 20.f;
constexpr float WORLD_SIZE_Y = 30.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float MAX_CAMERA_SHAKE=10.f;

constexpr float UI_XMAX=1600.f;
constexpr float UI_YMaX=800.f;

constexpr float PERCENTAGE_OF_OBSTACLE=0.3f;



