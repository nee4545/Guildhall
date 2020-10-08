#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>		
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#define UNUSED(x) (void)(x);

extern DevConsole g_theConsole;

#ifdef TEST_MODE
#define LOG_ERROR(...) printf(Stringf(__VA_ARGS__) + std::string("\n"))
#else
#define LOG_ERROR(...) g_theConsole.PrintError(Stringf(__VA_ARGS__))
#endif

class JobSystem;

extern NamedProperties g_gameConfigBlackboard; 
extern EventSystem g_theEventSystem;
extern HWND g_hWnd;
extern JobSystem* g_theJobSystem;

const Vec2 ALIGN_CENTERED = Vec2( 0.5f, 0.5f );
const Vec2 ALIGN_BOTTOM_LEFT = Vec2( 0.0f, 0.0f );

enum Convensions
{
	CONVENTION_XRIGHT_YUP_ZBACK ,
	CONVENSION_XEAST_YNORTH_ZUP ,
};


