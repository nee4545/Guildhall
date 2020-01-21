#include "DevConsole.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"

void DevConsole::Startup()
{

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

void DevConsole::PrintString( const Rgba8& textColor, const std::string& devConsolePrintString )
{
	ColoredLine newLine;
	newLine.lineColor = textColor;
	newLine.text = devConsolePrintString;

	m_lines.push_back(newLine);

}

void DevConsole::Render( RenderContext& renderer, const Camera& camera, float lineHeight ) const
{
	renderer.BeginCamera(camera);
	AABB2 consoleBox = AABB2(camera.GetOrthoBottomLeft(),camera.GetOrthoTopRight());
	float dimensionOfBox = camera.GetOrthoTopRight().y - camera.GetOrthoBottomLeft().y;
	int numberOfLines = RoundDownToInt(dimensionOfBox/lineHeight);

	Vec2 startMins = camera.GetOrthoBottomLeft();
	int myStringIndex = (int)m_lines.size()-1;
	renderer.DrawAABB2D(consoleBox,Rgba8(100,100,100,200));
	for( int index =0; index<numberOfLines; index++ )
	{
		DrawTextTriangles2D(renderer,m_lines[myStringIndex].text,startMins,1.f,m_lines[myStringIndex].lineColor);
		myStringIndex--;
		if( myStringIndex<0 )
		{
			break;
		}
		startMins.y+=lineHeight;
	}
	renderer.EndCamera(camera);
}

void DevConsole::SetIsOpen( bool isOpen )
{
	isConsoleOpen = isOpen;
}

bool DevConsole::IsOpen() const
{
	return isConsoleOpen;
}
