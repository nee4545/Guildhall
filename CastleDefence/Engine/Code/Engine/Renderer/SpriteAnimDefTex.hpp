
#pragma once
#include <vector>
class Texture;


class SpriteAnimDefTex
{
public:
	SpriteAnimDefTex( int startSpriteIndex , int endSpriteIndex ,
		float durationSeconds );

	Texture* GetSpriteTextureAtTime( float seconds ) const;
	void SetAnimationDuration( float duration );
	std::vector <Texture*> m_animations;

private:
	std::vector <int> m_animFrames;
	int				m_startSpriteIndex = -1;
	int				m_endSpriteIndex = -1;
	float				m_durationSeconds = 1.f;
};