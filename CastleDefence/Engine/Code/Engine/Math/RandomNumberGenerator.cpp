#include "RandomNumberGenerator.hpp"
#include "Engine/Math/RawNoise.hpp"
#include "Engine/Math/Vec2.hpp"

int RandomNumberGenerator::GetRandomIntInRange(int minInclusive, int maxInclusive )
{
	int range= maxInclusive-minInclusive;
	return (minInclusive + rand()% (range+1));
}

int RandomNumberGenerator::GetRandomIntLessThan( int maxNotInclusive )
{
	return (rand()%maxNotInclusive);
}

//Not included for grading in assignment 2
float RandomNumberGenerator::GetRandomFloatZeroToOne()
{
	constexpr float scale= 1.f/ (float)RAND_MAX;
	return scale * (float)rand();
}

//Not included in grading assignment 2
float RandomNumberGenerator::GetRandomFloatInRange( float minInclusive, float maxInclusive )
{
	constexpr float scale= 1.f/ (float)RAND_MAX;
	return minInclusive + scale * (float)rand() / maxInclusive; 
}

int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	return Get1dNoiseUint(m_position++,m_seed) % maxNotInclusive;
}

int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	int range = maxInclusive - minInclusive +1;
	return minInclusive + RollRandomIntLessThan(range);
}

float RandomNumberGenerator::RollRandomFloatLessThan( float maxNotInclusive )
{
	constexpr float scale= 1.f/ (float)RAND_MAX;
	return scale * (float)Get1dNoiseUint(m_position++,m_seed) / maxNotInclusive;
}

float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	float scale = maxInclusive - minInclusive;
	return minInclusive + ( scale * ( float ) RollRandomFloatZeroToOneInclusive() );
}

float RandomNumberGenerator::RollRandomFloatZeroToOneInclusive()
{
	constexpr double scale = 1.f / ( double ) 0xFFFFFFFF;
	unsigned int randomBits = Get1dNoiseUint( m_position++ , m_seed );
	return ( float ) scale * ( float ) randomBits;
}

float RandomNumberGenerator::RollRandomFloatZeroToAlmostOne()
{
	constexpr float scale= 1.f/ (float)(RAND_MAX+1);
	int randomBits = Get1dNoiseUint( m_position++, m_seed );
	return scale * (float)randomBits;
}

bool RandomNumberGenerator::RollPercentChance( float probablityOfReturningTrue )
{
	float randomNumber = RollRandomFloatZeroToOneInclusive();
	if( randomNumber>=probablityOfReturningTrue )
	{
		return false;
	}
	else
	{
		return true;
	}
}

Vec2 RandomNumberGenerator::RollRandomDirection2D()
{
	Vec2 toReturn;
	toReturn.x = RollRandomFloatZeroToOneInclusive();
	toReturn.y = RollRandomFloatZeroToOneInclusive();

	toReturn.Normalize();

	return toReturn;
}

void RandomNumberGenerator::Reset( unsigned int seed/*=0*/ )
{
	m_seed = seed;
	m_position =0;
}
