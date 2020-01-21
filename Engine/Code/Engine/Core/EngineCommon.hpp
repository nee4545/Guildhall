#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>		
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#define UNUSED(x) (void)(x);

extern NamedStrings g_gameConfigBlackboard; 
extern DevConsole g_theConsole;
extern EventSystem g_theEventSystem;
extern HWND g_hWnd;

const Vec2 ALIGN_CENTERED = Vec2( 0.5f, 0.5f );
const Vec2 ALIGN_BOTTOM_LEFT = Vec2( 0.0f, 0.0f );
