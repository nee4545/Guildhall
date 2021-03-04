#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "SpriteSheet.hpp"
#include "SpriteDefinition.hpp"


SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType ):m_spriteSheet(sheet),m_startSpriteIndex(startSpriteIndex),m_endSpriteIndex(endSpriteIndex),m_durationSeconds(durationSeconds)
{
	m_playbackType = playbackType;
}

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	float interval = m_durationSeconds/(m_endSpriteIndex+1-m_startSpriteIndex);
	int index = (int)(fmod(seconds,m_durationSeconds)/interval)+m_startSpriteIndex;
	return m_spriteSheet.GetSpriteDefinition(index);
}
