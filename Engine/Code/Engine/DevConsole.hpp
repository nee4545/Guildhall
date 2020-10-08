#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <mutex>

class Camera;
class RenderContext;
class BitmapFont;
struct AABB2;

struct ColoredLine
{
	Rgba8 lineColor;
	std::string text;
};

class DevConsole
{
public:
	DevConsole();
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	void Update( float deltaSeconds );

	void PrintString( const Rgba8& textColor , const std::string& devConsolePrintString );
	void PrintError( const std::string& errorMessage );
	void Render( RenderContext& renderer , Camera& camera , float textSize , float lineHeight ) const;
	void ProcessInput();
	void HandleCarrotChanges();
	void HandleCommandHistoryRequest();
	void SetCarrotUsingMouse();
	bool IsCommandInHistory( std::string command );
	void ProcessCommand( std::string& command );
	void ProcessCommand( std::string& command , EventArgs& commandArgs );
	void InitializeCommands();
	void HandleTextSelection();
	void HandleCopyPaste();
	std::string GetClipBoardData();

	void SetIsOpen( bool isOpen );
	bool IsOpen() const;
	bool isSelectedTextAfterCursor = false;
	float m_carrotBlinkTime = 1.f;
	int m_carrotOffest = 0;
	int m_commandHistoryIndex = 0;
	float m_textSize = 0;
	int m_selectedTextStart = 0;
	int m_selectedTextEnd = 0;

	void TakeCamera( Camera* camera );
	inline void SetTextSize( float size ) { m_textSize = size; }

	AABB2* m_selectedText;

private:
	bool isConsoleOpen = false;
	std::vector<ColoredLine> m_lines;
	std::string m_input = "";
	std::vector<std::string> m_commands;
	std::vector<std::string> m_commandHistory;
	Camera* m_devConsoleCamera = nullptr;
	//std::mutex m_mutex;

};