#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/XmlUtils.hpp"

class Shader;

class ShaderState
{
public:
	ShaderState() {};
	ShaderState( tinyxml2::XMLElement const& node );
	~ShaderState();

	void SetupFromXML( tinyxml2::XMLElement const& node );

	Shader* GetCurrentShader() const { return m_shader; }

	void	SetCurrentShader( Shader* const shader );

public:
	Shader* m_shader;

	BlendMode	m_blendMode = BlendMode::ALPHA;
	eCompareOp	m_depthTest = COMPARE_LEQUAL;

	bool		m_writeDepth = true;
	bool		m_windingOrderCounterClockwise = true;
	eCullMode	m_culling = eCullMode::CULL_NONE;
	eFillMode   m_fillmode= eFillMode::SOLID;
};
