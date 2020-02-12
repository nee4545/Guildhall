#pragma once
#include <string>
#include <vector>
#include "Engine/Core/Rgba8.hpp"
class Camera;
class RenderContext;

struct ColoredLine
{
	Rgba8 lineColor;
	std::string text;
};

class DevConsole
{
public:
	DevConsole(){};
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void PrintString( const Rgba8& textColor, const std::string& devConsolePrintString );
	void Render( RenderContext& renderer,  Camera& camera, float lineHeight ) const;

	void SetIsOpen( bool isOpen );
	bool IsOpen() const;

private:
	bool isConsoleOpen = false;
	std::vector<ColoredLine> m_lines;

};