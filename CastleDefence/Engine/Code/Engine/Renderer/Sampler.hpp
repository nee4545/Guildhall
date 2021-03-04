#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <string>

struct ID3D11SamplerState;
class RenderContext;

enum eSamplerType
{
	SAMPLER_POINT,
	SAMPLER_BILINEAR,
	SAMPLER_TOTAL
};


class Sampler
{
public:
	RenderContext* m_owner;
	ID3D11SamplerState* m_handle;

public:
	Sampler( RenderContext* ctx , eSamplerType type );
	~Sampler();
};