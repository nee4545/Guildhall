#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"

constexpr float CLIENT_ASPECT = 16.f/9.f; // We are requesting a 2:1 aspect (square) window area
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern InputSystem* g_theInput;

constexpr int NUM_STARTING_ASTEROIDS = 6;
constexpr int MAX_ASTEROIDS = 12;
constexpr int MAX_BULLETS = 20;
constexpr float WORLD_SIZE_X = 20.f;
constexpr float WORLD_SIZE_Y = 30.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;
constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;

constexpr int MAX_DEBRIS= 1000;
constexpr float MAX_CAMERA_SHAKE=10.f;
constexpr int MAX_BUGS=100; // bettles and wasps

constexpr float UI_XMAX=1600.f;
constexpr float UI_YMaX=800.f;

constexpr float PERCENTAGE_OF_OBSTACLE=0.3f;



