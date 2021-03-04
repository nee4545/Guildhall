#include "Engine/Renderer/SpriteAnimDefTex.hpp"

SpriteAnimDefTex::SpriteAnimDefTex( int startSpriteIndex , int endSpriteIndex , float durationSeconds )
{
	m_startSpriteIndex = startSpriteIndex;
	m_endSpriteIndex = endSpriteIndex;
	m_durationSeconds = durationSeconds;
}

Texture* SpriteAnimDefTex::GetSpriteTextureAtTime( float seconds ) const
{
	float interval = m_durationSeconds / ( m_endSpriteIndex + 1 - m_startSpriteIndex );
	int index = ( int ) ( fmod( seconds , m_durationSeconds ) / interval ) + m_startSpriteIndex;
	return m_animations[ index ];
}

void SpriteAnimDefTex::SetAnimationDuration( float duration )
{
	m_durationSeconds = duration;
}
