
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

extern RenderContext* g_theRenderer;

SpriteAnimSet::SpriteAnimSet( const tinyxml2::XMLElement& animElement )
{
	std::string rootToAppend = "Data/Images/";
	std::string spriteSheet =rootToAppend + animElement.Attribute( "spriteSheet" );
	IntVec2 spriteDimensions;
	spriteDimensions.SetFromText( animElement.Attribute( "spriteLayout" ) );
	Texture* spriteTexture = g_theRenderer->GetOrCreateTextureFromFile( spriteSheet.c_str() );
	m_spriteSheet = new SpriteSheet( *spriteTexture , spriteDimensions );
	float animDuration = (float)atof( animElement.Attribute( "fps" ) );

	const tinyxml2::XMLElement* animChildElements = animElement.FirstChildElement();
	while ( animChildElements )
	{
		std::string name = animChildElements->Attribute( "name" );
		std::string indices = animChildElements->Attribute( "spriteIndexes" );
		Strings spriteIndexes = SplitStringOnDelimiter( indices , ',' );
		int startIndex = atoi( spriteIndexes[ 0 ].c_str() );
		int endIndex = atoi( spriteIndexes[ spriteIndexes.size() - 2 ].c_str() );
		SpriteAnimDefinition* newDefinition = new SpriteAnimDefinition( *m_spriteSheet , startIndex , endIndex , animDuration );
		m_animDefinitions[ name ] = newDefinition;
		animChildElements = animChildElements->NextSiblingElement();

	}


}

SpriteAnimDefinition* SpriteAnimSet::GetDefinition( const std::string& name )
{
	return m_animDefinitions[ name ];
}
