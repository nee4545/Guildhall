#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EventSystem.hpp"

class Camera;
class RenderContext;
class BitmapFont;

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
	void Update(float deltaSeconds);

	void PrintString( const Rgba8& textColor, const std::string& devConsolePrintString );
	void Render( RenderContext& renderer,  Camera& camera,float textSize, float lineHeight ) const;
	void ProcessInput();
	void HandleCarrotChanges();
	void ProcessCommand(std::string& command);
	void InitializeCommands();

	void SetIsOpen( bool isOpen );
	bool IsOpen() const;
	float m_carrotBlinkTime = 1.f;
	int m_carrotOffest = 0;

private:
	bool isConsoleOpen = false;
	std::vector<ColoredLine> m_lines;
	std::string m_input = "";
	std::vector<std::string> m_commands;

};