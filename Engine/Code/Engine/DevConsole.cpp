#include "DevConsole.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Input/InputSystem.hpp"

extern BitmapFont* g_theBitMapFont;
extern InputSystem* g_theInput;

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

void DevConsole::Update(float deltaSeconds)
{
	if ( !IsOpen() )
	{
		return;
	}

	ProcessInput();
	HandleCommandHistoryRequest();
	HandleCarrotChanges();

	m_carrotBlinkTime -= deltaSeconds;

	if ( m_carrotBlinkTime <= 0.f )
	{
		m_carrotBlinkTime = 1.f;
	}


}

void DevConsole::PrintString( const Rgba8& textColor, const std::string& devConsolePrintString )
{
	ColoredLine newLine;
	newLine.lineColor = textColor;
	newLine.text = devConsolePrintString;

	m_lines.push_back(newLine);

}

void DevConsole::Render( RenderContext& renderer,  Camera& camera,float textSize, float lineHeight ) const
{
	
	renderer.BeginCamera(camera);
	AABB2 consoleBox = AABB2(camera.GetOrthoBottomLeft(),camera.GetOrthoTopRight());
	float dimensionOfBox = camera.GetOrthoTopRight().y - camera.GetOrthoBottomLeft().y;
	consoleBox.CarveBoxOffTop( 0.8f );
	int numberOfLines = RoundDownToInt(dimensionOfBox/lineHeight);

	Vec2 startMins = consoleBox.mins;
	int myStringIndex = (int)m_lines.size()-1;
	renderer.DrawAABB2D(consoleBox,Rgba8(100,100,100,200));
	std::vector<Vertex_PCU> textVerts;
	for( int index =0; index<numberOfLines; index++ )
	{
		if ( m_lines.size() == 0 )
		{
			break;
		}
	
		g_theBitMapFont->AddVertsForText2D( textVerts,startMins,textSize, m_lines[ myStringIndex ].text, m_lines[ myStringIndex ].lineColor );

		renderer.BindTexture( g_theBitMapFont->GetTexture() );
		renderer.DrawVertexArray( textVerts );
		renderer.BindTexture( nullptr );

		myStringIndex--;
		if( myStringIndex<0 )
		{
			break;
		}
		startMins.y+=lineHeight*textSize;
	}

	AABB2 textBox = AABB2( camera.GetOrthoBottomLeft() , camera.GetOrthoTopRight() );
	textBox.CarveBoxOffBottom( 0.2f );
	renderer.DrawAABB2D( textBox,Rgba8(100,0,0,100) );

	AABB2 carrot = AABB2( Vec2( 0.f , 0.2f ) , Vec2( 1.5f , textSize+0.5f ) );
	carrot.Translate( Vec2( (m_input.size()+m_carrotOffest)*textSize , 0.f ) );


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
	
	renderer.EndCamera(camera);
}

void DevConsole::ProcessInput()
{
	if ( g_theInput->m_characters.size()>0 )
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

		if(m_carrotOffest==0 )
		{ 
			m_input += g_theInput->m_characters.front();
		}
		else
		{
			std::string temp = "";
			temp += g_theInput->m_characters.front();
			m_input.insert( m_input.size() + m_carrotOffest , temp );
		}
	}

	if ( g_theInput->WasKeyJustPressed( 0x2E ) )
	{
		if ( m_carrotOffest == 0 )
		{
			/*if ( m_input.size() > 0 )
			{
				m_input.erase( m_input.size() - 1 , 1 );
			}*/
		}
		else
		{
			if ( m_input.size() + m_carrotOffest >= 0 )
			{
				m_input.erase( m_input.size() + m_carrotOffest , 1 );
				m_carrotOffest++;
			}

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
			m_carrotOffest = ( m_input.size() )*-1;
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
		if ( m_commandHistoryIndex+1 <= m_commandHistory.size()-1 )
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

	Vec2 mousePos = m_devConsoleCamera->ClientToWordPosition( g_theInput->GetCurrentMousePosition() );



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

void DevConsole::ProcessCommand(std::string& command)
{
	for ( int index = 0; index < m_commands.size(); index++ )
	{
		if ( m_commands[ index ] == command )
		{
			g_theEventSystem.FireEvent( command , g_gameConfigBlackboard );
			if ( !IsCommandInHistory( command ) )
			{
				m_commandHistory.push_back( command );
			}
			return;
		}

	}

	PrintString( Rgba8( 100 , 0 , 0 , 255 ) , "Invalid command: " + command );
}

void DevConsole::InitializeCommands()
{
	m_commands.push_back( "quit" );
	m_commands.push_back( "help" );
	m_commands.push_back( "dummy" );
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
