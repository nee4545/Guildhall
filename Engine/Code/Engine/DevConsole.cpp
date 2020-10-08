#include "DevConsole.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/NamedProperties.hpp"

extern BitmapFont* g_theBitMapFont;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;

std::mutex g_consoleMutex;

DevConsole::DevConsole()
{

}

void DevConsole::Startup()
{
	InitializeCommands();
}

void DevConsole::BeginFrame()
{

}

void DevConsole::EndFrame()
{

}

void DevConsole::Shutdown()
{

}

void DevConsole::Update( float deltaSeconds )
{
	if ( !IsOpen() )
	{
		return;
	}

	ProcessInput();
	HandleCommandHistoryRequest();
	HandleCarrotChanges();
	SetCarrotUsingMouse();
	HandleTextSelection();

	m_carrotBlinkTime -= deltaSeconds;

	if ( m_carrotBlinkTime <= 0.f )
	{
		m_carrotBlinkTime = 1.f;
	}


}

void DevConsole::PrintString( const Rgba8& textColor , const std::string& devConsolePrintString )
{
	ColoredLine newLine;
	newLine.lineColor = textColor;
	newLine.text = devConsolePrintString;
	//g_consoleMutex.lock();
	m_lines.push_back( newLine );
	//g_consoleMutex.unlock();
}

void DevConsole::PrintError( const std::string& errorMessage )
{
	ColoredLine newLine;
	newLine.lineColor = Rgba8( 255 , 0 , 0 , 255 );
	newLine.text = errorMessage;

	m_lines.push_back( newLine );

	SetIsOpen( true );
}

void DevConsole::Render( RenderContext& renderer , Camera& camera , float textSize , float lineHeight ) const
{

	renderer.BeginCamera( camera );
	renderer.BindDepthStencil( nullptr );
	AABB2 consoleBox = AABB2( camera.GetOrthoBottomLeft() , camera.GetOrthoTopRight() );
	float dimensionOfBox = camera.GetOrthoTopRight().y - camera.GetOrthoBottomLeft().y;
	consoleBox.CarveBoxOffTop( 0.9f );
	int numberOfLines = RoundDownToInt( dimensionOfBox / lineHeight );

	Vec2 startMins = consoleBox.mins;
	int myStringIndex = ( int ) m_lines.size() - 1;
	renderer.DrawAABB2D( consoleBox , Rgba8( 100 , 100 , 100 , 200 ) );
	std::vector<Vertex_PCU> textVerts;
	for ( int index = 0; index < numberOfLines; index++ )
	{
		if ( m_lines.size() == 0 )
		{
			break;
		}

		g_theBitMapFont->AddVertsForText2D( textVerts , startMins , textSize , m_lines[ myStringIndex ].text , m_lines[ myStringIndex ].lineColor );

		renderer.BindTexture( g_theBitMapFont->GetTexture() );
		renderer.DrawVertexArray( textVerts );
		renderer.BindTexture( nullptr );

		myStringIndex--;
		if ( myStringIndex < 0 )
		{
			break;
		}
		startMins.y += lineHeight * textSize;
	}

	AABB2 textBox = AABB2( camera.GetOrthoBottomLeft() , camera.GetOrthoTopRight() );
	textBox.CarveBoxOffBottom( 0.1f );
	renderer.DrawAABB2D( textBox , Rgba8( 100 , 0 , 0 , 100 ) );

	AABB2 carrot = AABB2( Vec2( 0.f , 0.2f ) , Vec2( 0.5f , textSize + 0.5f ) );
	carrot.Translate( Vec2( ( m_input.size() + m_carrotOffest ) * textSize , 0.f ) );


	std::vector<Vertex_PCU> inputTextVerts;

	if ( m_input.size() > 0 )
	{
		g_theBitMapFont->AddVertsForText2D( inputTextVerts , textBox.mins , textSize , m_input , Rgba8( 100 , 0 , 0 , 255 ) );
		renderer.BindTexture( g_theBitMapFont->GetTexture() );
		renderer.DrawVertexArray( inputTextVerts );
		renderer.BindTexture( nullptr );
	}

	if ( m_carrotBlinkTime > 0.5f )
	{
		renderer.DrawAABB2D( carrot , Rgba8( 100 , 0 , 0 , 100 ) );
	}

	if ( m_selectedText != nullptr )
		renderer.DrawAABB2D( *m_selectedText , Rgba8( 100 , 100 , 100 , 100 ) );

	renderer.EndCamera( camera );
}

void DevConsole::ProcessInput()
{

	if ( g_theInput->IsKeyPressed( 0x11 ) && g_theInput->WasKeyJustPressed( 'V' ) )
	{
		HandleCopyPaste();
		return;
	}

	if ( g_theInput->m_characters.size() > 0 )
	{

		if ( g_theInput->m_characters.front() == 27 )
		{
			if ( m_input != "" )
			{
				m_input = "";
				m_carrotOffest = 0;
				return;
			}
			else
			{
				m_input = "";
				SetIsOpen( false );
				m_carrotOffest = 0;
				return;

			}
		}

		if ( g_theInput->m_characters.front() == '`' || g_theInput->m_characters.front() == '~' )
		{
			m_input = "";
			m_carrotOffest = 0;
			return;
		}

		if ( g_theInput->m_characters.front() == 13 )
		{

			if ( m_input.size() > 0 )
			{
				PrintString( Rgba8( 255 , 255 , 255 , 255 ) , m_input );
				ProcessCommand( m_input );
			}
			m_input = "";
			m_carrotOffest = 0;
			return;
		}

		if ( g_theInput->m_characters.front() == 8 )
		{

			if ( m_selectedText != nullptr )
			{
				m_input.erase( m_selectedTextStart , ( m_selectedTextEnd - m_selectedTextStart ) );
				m_selectedText = nullptr;
				if ( m_carrotOffest < 0 )
				{
					if ( isSelectedTextAfterCursor )
					{
						m_carrotOffest += m_selectedTextEnd - m_selectedTextStart;
						isSelectedTextAfterCursor = false;
					}
				}
				return;
			}

			if ( m_carrotOffest == 0 )
			{
				if ( m_input.size() > 0 )
				{
					m_input.erase( m_input.size() - 1 , 1 );
				}
			}
			else
			{
				if ( m_input.size() + m_carrotOffest > 0 )
				{
					m_input.erase( m_input.size() + m_carrotOffest - 1 , 1 );
				}
			}
			return;
		}

		if ( m_carrotOffest == 0 )
		{
			if ( m_selectedText != nullptr )
			{
				m_input.erase( m_selectedTextStart , ( m_selectedTextEnd - m_selectedTextStart ) );
				m_selectedText = nullptr;
				if ( m_carrotOffest < 0 )
				{
					if ( isSelectedTextAfterCursor )
					{
						m_carrotOffest += m_selectedTextEnd - m_selectedTextStart;
						isSelectedTextAfterCursor = false;
					}
				}
			}
			m_input += g_theInput->m_characters.front();
		}
		else
		{

			if ( m_selectedText != nullptr )
			{
				m_input.erase( m_selectedTextStart , ( m_selectedTextEnd - m_selectedTextStart ) );
				m_selectedText = nullptr;
				if ( m_carrotOffest < 0 )
				{
					if ( isSelectedTextAfterCursor )
					{
						m_carrotOffest += m_selectedTextEnd - m_selectedTextStart;
						isSelectedTextAfterCursor = false;
					}
				}
			}
			std::string temp = "";
			temp += g_theInput->m_characters.front();
			m_input.insert( m_input.size() + m_carrotOffest , temp );
		}

	}

	if ( g_theInput->WasKeyJustPressed( 0x2E ) )
	{

		if ( m_selectedText != nullptr )
		{
			m_input.erase( m_selectedTextStart , ( m_selectedTextEnd - m_selectedTextStart ) );
			m_selectedText = nullptr;

			if ( m_carrotOffest < 0 )
			{
				if ( isSelectedTextAfterCursor )
				{
					m_carrotOffest += m_selectedTextEnd - m_selectedTextStart;
					isSelectedTextAfterCursor = false;
				}
			}
			return;
		}

		if ( m_carrotOffest == 0 )
		{
			return;
		}

		if ( m_input.size() + m_carrotOffest >= 0 )
		{
			m_input.erase( m_input.size() + m_carrotOffest , 1 );
			m_carrotOffest++;
		}

	}

	if ( g_theInput->WasKeyJustPressed( 0x23 ) )
	{
		if ( m_input != "" )
		{
			m_carrotOffest = 0;
		}
	}

	if ( g_theInput->WasKeyJustPressed( 0x24 ) )
	{
		if ( m_input != "" )
		{
			m_carrotOffest = ( int ) ( m_input.size() ) * -1;
		}
	}
}

void DevConsole::HandleCarrotChanges()
{
	if ( g_theInput->WasKeyJustPressed( 0x27 ) )
	{
		if ( m_carrotOffest < 0 )
		{
			m_carrotOffest++;
		}
	}

	if ( g_theInput->WasKeyJustPressed( 0x25 ) )
	{
		if ( m_input.size() > 0 )
		{
			if ( abs( m_carrotOffest ) < m_input.size() )
			{
				m_carrotOffest--;
			}
		}
	}
}

void DevConsole::HandleCommandHistoryRequest()
{
	if ( m_commandHistory.size() <= 0 )
	{
		return;
	}

	if ( g_theInput->WasKeyJustPressed( 0x26 ) )
	{
		if ( m_commandHistoryIndex + 1 <= m_commandHistory.size() - 1 )
		{
			m_commandHistoryIndex++;
		}

		m_input = m_commandHistory[ m_commandHistoryIndex ];
		m_carrotOffest = 0;

	}

	if ( g_theInput->WasKeyJustPressed( 0x28 ) )
	{

		if ( m_commandHistoryIndex > 0 )
		{
			if ( m_commandHistory.size() > 1 )
			{
				m_commandHistoryIndex--;
			}
		}

		m_input = m_commandHistory[ m_commandHistoryIndex ];
		m_carrotOffest = 0;

		if ( m_commandHistoryIndex - 1 >= 0 )
		{
			m_commandHistoryIndex--;
		}
	}
}

void DevConsole::SetCarrotUsingMouse()
{
	if ( m_input == "" )
	{
		return;
	}

	Vec2 mousePos = m_devConsoleCamera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	AABB2 textBoc = AABB2( m_devConsoleCamera->GetOrthoBottomLeft() , m_devConsoleCamera->GetOrthoTopRight() );
	textBoc.CarveBoxOffBottom( 0.1f );

	if ( textBoc.IsPointInside( mousePos ) )
	{
		AABB2 actualTextBox = AABB2( textBoc.mins , Vec2( m_input.size() * m_textSize , 1.5f ) );

		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			if ( mousePos.x > actualTextBox.maxs.x )
			{
				m_carrotOffest = 0;
			}
		}

		if ( actualTextBox.IsPointInside( mousePos ) )
		{
			if ( g_theInput->WasLeftMouseButtonJustPressed() )
			{
				m_carrotOffest = RoundToNearestInt( ( actualTextBox.maxs.x - mousePos.x ) / m_textSize ) * -1;
			}
		}
	}

}

bool DevConsole::IsCommandInHistory( std::string command )
{
	if ( m_commandHistory.size() <= 0 )
	{
		return false;
	}

	for ( int index = 0; index < m_commandHistory.size(); index++ )
	{
		if ( m_commandHistory[ index ] == command )
		{
			return true;
		}
	}

	return false;
}

void DevConsole::ProcessCommand( std::string& command )
{
	Strings cmd = SplitStringOnDelimiter( command , ':' );
	EventArgs currentCommandArgs;
	std::string currentCommand;

	if ( cmd.size() == 2 )
	{
		currentCommand = cmd[ 0 ];
		std::string parameter = cmd[ 1 ];
		currentCommandArgs.SetValue( currentCommand , parameter );

		Strings parameters = SplitStringOnDelimiter( cmd[ 1 ] , '|' );

		for ( int i = 0; i < parameters.size(); i++ )
		{
			std::string s = parameters[ i ];
			Strings split = SplitStringOnDelimiter( s , '=' );

			if ( split.size() == 2 )
			{
				currentCommandArgs.SetValue( split[ 0 ] , split[ 1 ] );
			}
		}
	}

	for ( int i = 0; i < m_commands.size(); i++ )
	{
		if ( cmd.size() < 2 )
		{
			break;
		}

		if ( m_commands[ i ] == cmd[ 0 ] )
		{
			SoundID s = g_theAudio->CreateOrGetSound( "Data/Sounds/Success.wav" );
			g_theAudio->PlaySound( s );
			g_theEventSystem.FireEvent( currentCommand , currentCommandArgs );
			if ( !IsCommandInHistory( currentCommand ) )
			{
				m_commandHistory.push_back( currentCommand );
			}
			return;
		}
	}

	for ( int index = 0; index < m_commands.size(); index++ )
	{
		if ( m_commands[ index ] == command )
		{
			SoundID s = g_theAudio->CreateOrGetSound( "Data/Sounds/Success.wav" );
			g_theAudio->PlaySound( s );
			g_theEventSystem.FireEvent( command , g_gameConfigBlackboard );
			if ( !IsCommandInHistory( command ) )
			{
				m_commandHistory.push_back( command );
			}
			return;
		}

	}

	SoundID s = g_theAudio->CreateOrGetSound( "Data/Sounds/Error.wav" );
	g_theAudio->PlaySound( s );
	PrintString( Rgba8( 100 , 0 , 0 , 255 ) , "Invalid command: " + command );
}

void DevConsole::ProcessCommand( std::string& command , EventArgs& commandArgs )
{
	g_theEventSystem.FireEvent( command , commandArgs );
}

void DevConsole::InitializeCommands()
{
	m_commands.push_back( "quit" );
	m_commands.push_back( "help" );
	m_commands.push_back( "close" );
	m_commands.push_back( "set_physics_update_freq" );
	m_commands.push_back( "debug_render_enabled" );
	m_commands.push_back( "debug_add_world_point" );
	m_commands.push_back( "debug_add_screen_point" );
	m_commands.push_back( "debug_add_screen_quad" );
	m_commands.push_back( "debug_add_world_wiresphere" );
	m_commands.push_back( "debug_add_world_wirequad" );
	m_commands.push_back( "debug_add_world_billboardtext" );
	m_commands.push_back( "debug_add_screen_text" );
	m_commands.push_back( "light_set_ambient_color" );
	m_commands.push_back( "maps" );
	m_commands.push_back( "map" );
	m_commands.push_back( "StartServer" );
	m_commands.push_back( "Connect" );

	m_commands.push_back( "SendServerMessage" );
	m_commands.push_back( "SendClientMessage" );
	m_commands.push_back( "StopServer" );
	m_commands.push_back( "Disconnect" );
	m_commands.push_back( "OpenUDPPort" );
	m_commands.push_back( "SendUDPMessage" );
	m_commands.push_back( "CloseUDPPort" );

}

void DevConsole::HandleTextSelection()
{
	if ( m_input == "" )
	{
		m_selectedText = nullptr;
		isSelectedTextAfterCursor = false;
		return;
	}

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		m_selectedText = nullptr;
		return;
	}

	Vec2 mousePos = m_devConsoleCamera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	AABB2 textBoc = AABB2( m_devConsoleCamera->GetOrthoBottomLeft() , m_devConsoleCamera->GetOrthoTopRight() );
	textBoc.CarveBoxOffBottom( 0.1f );

	AABB2 actualTextBox = AABB2( textBoc.mins , Vec2( m_input.size() * m_textSize , 1.5f ) );

	if ( actualTextBox.IsPointInside( mousePos ) )
	{

		if ( g_theInput->IsLeftMouseButtonPressed() )
		{
			AABB2* selectedText = new AABB2( Vec2( mousePos.x , 0.2f ) , Vec2( ( m_input.size() + m_carrotOffest ) * m_textSize , 2.5f ) );
			m_selectedText = selectedText;
		}
	}

	if ( m_selectedText != nullptr )//Map index of aabb for the text to select
	{
		int textEndIndex = ( int ) m_input.size() + m_carrotOffest;
		int textStartIndex = RoundToNearestInt( ( m_selectedText->mins.x - m_selectedText->maxs.x ) / m_textSize );


		if ( textStartIndex > 0 )
		{
			m_selectedTextStart = textEndIndex;
			m_selectedTextEnd = m_selectedTextStart + textStartIndex;
			isSelectedTextAfterCursor = true;
		}
		else if ( textStartIndex < 0 )
		{
			m_selectedTextEnd = textEndIndex;
			m_selectedTextStart = m_selectedTextEnd + textStartIndex;
		}
		else
		{
			m_selectedText = nullptr;
			isSelectedTextAfterCursor = false;
		}
	}
}



void DevConsole::HandleCopyPaste()
{
	std::string clipBoardData = GetClipBoardData();

	if ( clipBoardData == "" )
	{
		return;
	}

	m_input += clipBoardData;
}

std::string DevConsole::GetClipBoardData()
{
	if ( !OpenClipboard( NULL ) )
		return "";
	if ( IsClipboardFormatAvailable( CF_TEXT ) )
	{
		HANDLE handleData = GetClipboardData( CF_TEXT );
		LPCSTR data = ( LPCSTR ) GlobalLock( handleData );
		std::string clipboardDataString = data;
		GlobalUnlock( handleData );
		CloseClipboard();
		return clipboardDataString;
	}
	else
	{
		CloseClipboard();
		return "";
	}
}

void DevConsole::SetIsOpen( bool isOpen )
{
	isConsoleOpen = isOpen;
}

bool DevConsole::IsOpen() const
{
	return isConsoleOpen;
}

void DevConsole::TakeCamera( Camera* camera )
{
	m_devConsoleCamera = camera;
}
