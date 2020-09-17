#pragma once
#include "Engine/ThirdParty/IMGUI/imgui.h"
#include "Engine/ThirdParty/IMGUI/imgui_impl_dx11.h"
#include "Engine/ThirdParty/IMGUI/imgui_impl_win32.h"

class Window;
class RenderContext;
class Camera;

class ImGuiSystem
{
public:
	ImGuiSystem( Window* window , RenderContext* context );
	~ImGuiSystem();

	void BeginFrame();
	void Render();
	void EndFrame();

	void StartUp();
	void ShutDown();

	RenderContext* m_renderContext = nullptr;
	Window* m_window = nullptr;
	Camera* m_camera = nullptr;
};