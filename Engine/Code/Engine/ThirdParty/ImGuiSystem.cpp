#include "Engine/ThirdParty/ImGuiSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Core/D3D11Common.hpp"

ImGuiSystem* g_theGUI = nullptr;

ImGuiSystem::ImGuiSystem( Window* window , RenderContext* context )
{
	m_window = window;
	m_renderContext = context;

	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 1920.f , 1080.f ) );
	m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 0 ) );
}

ImGuiSystem::~ImGuiSystem()
{
	
}

void ImGuiSystem::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
}

void ImGuiSystem::Render()
{
	m_renderContext->BeginCamera( *m_camera );
	ImGui::Render();
	ImDrawData* data = ImGui::GetDrawData();
	ImGui_ImplDX11_RenderDrawData( data );

	m_renderContext->EndCamera( *m_camera );
}

void ImGuiSystem::EndFrame()
{

}

void ImGuiSystem::StartUp()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	UNUSED( io );
	ImGui_ImplWin32_Init( m_window->m_hwnd );
	ImGui_ImplDX11_Init( m_renderContext->m_device , m_renderContext->m_context );
	ImGui::StyleColorsDark();
}

void ImGuiSystem::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
