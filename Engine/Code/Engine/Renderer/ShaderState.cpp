#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

ShaderState::ShaderState( tinyxml2::XMLElement const& node )
{
	m_depthTest = ( eCompareOp ) ParseXmlAttribute( node , "DepthTest" , ( int ) m_depthTest );
	m_blendMode = ( BlendMode ) ParseXmlAttribute( node , "BlendMode" , ( int ) m_blendMode );
	m_writeDepth = ParseXmlAttribute( node , "WriteDepth" , ( bool ) m_writeDepth );

	m_windingOrderCounterClockwise = ParseXmlAttribute( node , "WindingOrder" ,  true);
	m_culling = ( eCullMode ) ParseXmlAttribute( node , "CullMode" , ( int ) m_culling );
	m_fillmode = ( eFillMode ) ParseXmlAttribute( node , "FillMode" , ( int ) m_fillmode );

	std::string shaderPath = ParseXmlAttribute( node , "Shader" , "" );

	m_shader = g_theRenderer->GetOrCreateShader( shaderPath.c_str() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ShaderState::~ShaderState()
{
	m_shader = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ShaderState::SetupFromXML( tinyxml2::XMLElement const& node )
{
	m_depthTest = ( eCompareOp ) ParseXmlAttribute( node , "DepthTest" , ( int ) m_depthTest );
	m_blendMode = ( BlendMode ) ParseXmlAttribute( node , "BlendMode" , ( int ) m_blendMode );
	m_writeDepth =  ParseXmlAttribute( node , "WriteDepth" , true );

	m_windingOrderCounterClockwise =  ParseXmlAttribute( node , "WindingOrder" ,  true );
	m_culling = ( eCullMode ) ParseXmlAttribute( node , "CullMode" , ( int ) m_culling );
	m_fillmode = ( eFillMode ) ParseXmlAttribute( node , "FillMode" , ( int ) m_fillmode );

	std::string shaderPath = ParseXmlAttribute( node , "Shader" , "" );

	m_shader = g_theRenderer->GetOrCreateShader( shaderPath.c_str() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ShaderState::SetCurrentShader( Shader* const shader )
{
	m_shader = shader;
}
