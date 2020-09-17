#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Platform/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/ThirdParty/ImGuiSystem.hpp"
#include "Engine/ThirdParty/IMGUI/imgui.h"


static TCHAR const* WND_CLASS_NAME = L"Simple Window Class";

extern ImGuiSystem* g_theGUI;
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND windowHandle , UINT wmMessageCode , WPARAM wParam , LPARAM lParam );

static LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle , UINT wmMessageCode , WPARAM wParam , LPARAM lParam )
{
	Window* window = ( Window* )::GetWindowLongPtr( windowHandle , GWLP_USERDATA );
	
	bool handled = ImGui_ImplWin32_WndProcHandler( windowHandle , wmMessageCode , wParam , lParam );

	//ip->HandleKeyPressed((unsigned char)wParam);
	switch ( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{
		//make window do it
		//g_theapp->HandleQuitRequested();
		window->QuitWindow();
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	case WM_ACTIVATE:
	{

		/*InputSystem* ip = window->GetInputSystem();

		if ( wParam == 1 || wParam == 2 )
		{
			if(ip!=nullptr )
			ip->ClipSystemCursor();
		}
		*/
		break;
	}

	case WM_MOUSEWHEEL:
	{
		InputSystem* ip = window->GetInputSystem();
		ip->SetMouseWheelData( ( int ) GET_WHEEL_DELTA_WPARAM( wParam ) );
		break;
	}


	case WM_LBUTTONUP:
	{
		InputSystem* ip = window->GetInputSystem();
		ip->HandleLeftMouseButtonReleased();
		break;
	}

	case WM_LBUTTONDOWN:
	{
		InputSystem* ip = window->GetInputSystem();
		ip->HandleLeftMouseButtonPressed();
		break;
	}

	case WM_RBUTTONDOWN:
	{
		InputSystem* ip = window->GetInputSystem();
		ip->HandelRightMouseButtonPressed();
		break;
	}

	case WM_RBUTTONUP:
	{
		InputSystem* ip = window->GetInputSystem();
		ip->HandleRightMouseButtonReleased();
		break;
	}

	case WM_CHAR:
	{
		char character = ( char ) wParam;
		InputSystem* ip = window->GetInputSystem();
		ip->PushCharacter( character );
		break;
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		unsigned char asKey = ( unsigned char ) wParam;
		InputSystem* ip = window->GetInputSystem();
		ip->HandleKeyPressed( asKey );

		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char asKey = ( unsigned char ) wParam;
		InputSystem* ip = window->GetInputSystem();
		ip->HandleKeyReleased( asKey );

		//			#SD1ToDo: Tell the App and InputSystem about this key-released event...
		break;
	}


	}
	if ( g_theGUI != nullptr )
	{
		const ImGuiIO& IO = ImGui::GetIO();
		if ( handled || IO.WantCaptureMouse )
		{
			// DO nothing
		}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle , wmMessageCode , wParam , lParam );
}


static void RegisterWindowClass()
{
	// Define a window style/class
	HICON icon = LoadIcon( NULL , IDI_QUESTION );

	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription , 0 , sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = icon;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = WND_CLASS_NAME;
	::RegisterClassEx( &windowClassDescription );
}

static void UnregisterWindowClass()
{
	::UnregisterClass( WND_CLASS_NAME , GetModuleHandle( NULL ) );
}


Window::Window()
{
	RegisterWindowClass();
}

Window::~Window()
{
	UnregisterWindowClass();
}

bool Window::Open( std::string const& title , float clientAspect , float ratioOfHeight )
{

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle , &desktopRect );
	float desktopWidth = ( float ) ( desktopRect.right - desktopRect.left );
	float desktopHeight = ( float ) ( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	float maxClientFractionOfDesktop = ratioOfHeight;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if ( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * ( desktopWidth - clientWidth );
	float clientMarginY = 0.5f * ( desktopHeight - clientHeight );
	RECT clientRect;
	clientRect.left = ( int ) clientMarginX;
	clientRect.right = clientRect.left + ( int ) clientWidth;
	clientRect.top = ( int ) clientMarginY;
	clientRect.bottom = clientRect.top + ( int ) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect , windowStyleFlags , FALSE , windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP() , 0 , title.c_str() , -1 , windowTitle , sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );

	HWND hWnd = CreateWindowEx(
		windowStyleExFlags ,
		WND_CLASS_NAME ,
		windowTitle ,
		windowStyleFlags ,
		windowRect.left ,
		windowRect.top ,
		windowRect.right - windowRect.left ,
		windowRect.bottom - windowRect.top ,
		NULL ,
		NULL ,
		( HINSTANCE ) ::GetModuleHandle(NULL) ,
		NULL );

	::SetWindowLongPtr( hWnd , GWLP_USERDATA , ( LONG_PTR ) this );

	if ( hWnd == nullptr )
	{
		return false;
	}

	ShowWindow( hWnd , SW_SHOW );
	SetForegroundWindow( hWnd );
	SetFocus( hWnd );


	HCURSOR cursor = LoadCursor( NULL , IDC_ARROW );
	SetCursor( cursor );

	//SetWindowLong( hWnd , GWL_EXSTYLE , WS_EX_LAYERED );
	//SetLayeredWindowAttributes( hWnd , RGB( 0 , 0 , 0 ) , 128,LWA_ALPHA );

	m_width = (int)clientWidth;
	m_height = (int)clientHeight;
	m_hwnd = ( void* ) hWnd;
	return true;
}

void Window::Close()
{
	HWND hwnd = ( HWND ) m_hwnd;
	if ( NULL == hwnd )
	{
		return;
	}

	::DestroyWindow( hwnd );
	m_hwnd = nullptr;
}

void Window::SetInputSysten( InputSystem* input )
{
	m_input = input;
}

Vec2 Window::GetCentre()
{
	RECT centre; 
	GetClientRect( (HWND)m_hwnd,&centre );

	Vec2 c;
	c.x = ( float ) ( centre.right - centre.left ) * 0.5f;
	c.y = ( float ) ( centre.bottom - centre.top ) * 0.5f;

	return c;
}

void Window::QuitWindow()
{
	m_quitRequested = true;
}

void Window::BeginFrame()
{
	MSG queuedMessage;
	for ( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage , NULL , 0 , 0 , PM_REMOVE );
		if ( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}
