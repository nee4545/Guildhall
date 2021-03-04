#pragma once
#include <cstdlib>
struct Vec2;

class RandomNumberGenerator
{
public:
	int GetRandomIntLessThan( int maxNotInclusive );
	int GetRandomIntInRange( int minInclusive, int maxInclusive );

	float GetRandomFloatZeroToOne();
	float GetRandomFloatInRange( float minInclusive, float maxInclusive );

	//New methods that use noise and seed
	int RollRandomIntLessThan(int maxNotInclusive);
	int RollRandomIntInRange(int minInclusive,int maxInclusive);
	float RollRandomFloatLessThan(float maxNotInclusive);
	float RollRandomFloatInRange(float minInclusive, float maxInclusive);
	float RollRandomFloatZeroToOneInclusive();
	float RollRandomFloatZeroToAlmostOne();
	bool RollPercentChance(float probablityOfReturningTrue);
	Vec2 RollRandomDirection2D();

	void Reset(unsigned int seed=0);

private:
	unsigned int m_seed = 0;
	int m_position = 0;
	
};