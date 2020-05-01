#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/D3D11Common.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/ThirdParty/tinyxml2.h"

extern RenderContext * g_theRenderer;

Material::~Material()
{
	if ( m_ubo != nullptr )
	{
		delete m_ubo;
		m_ubo = nullptr;
	}

	if ( m_shaderState != nullptr )
	{
		delete m_shaderState;
		m_shaderState = nullptr;
	}

	for ( auto& x : m_samplersPerSlot )
	{
		if ( x.second != nullptr )
		{
			delete x.second;
			x.second = nullptr;
		}
	}
}


void Material::CreateFromFile( const char* xmlFilePath )
{
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );

	if ( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		ERROR_AND_DIE( "XML FILE DID NOT LOAD" );
		return;
	}

	tinyxml2::XMLElement* materialDefinition = xmlDocument.RootElement();

	std::string shaderState = ParseXmlAttribute( *materialDefinition , "ShaderState" , "null" );
	tinyxml2::XMLDocument shaderDoc;
	shaderDoc.LoadFile( shaderState.c_str() );

	tinyxml2::XMLElement* shaderStateDef = shaderDoc.RootElement();

	m_shaderState = new ShaderState();
	m_shaderState->SetupFromXML( *shaderStateDef );

	/*std::string shader = ParseXmlAttribute( *materialDefinition , "Shader" , "null" );
	m_shader = g_theRenderer->GetOrCreateShader( shader.c_str() );*/

	materialDefinition = materialDefinition->FirstChildElement();

	while ( materialDefinition )
	{
		std::string childElementName = materialDefinition->Value();

		if ( childElementName == "Texture" )
		{
			std::string texPath = ParseXmlAttribute( *materialDefinition , "texPath" , "Invalid Name" );
			int			bindSlot = ParseXmlAttribute( *materialDefinition , "bindSlot" , 0 );

			Texture* temp = g_theRenderer->GetOrCreateTextureFromFile( texPath.c_str() );
			m_texturePerSlot[ bindSlot ] = temp;
		}

		if ( childElementName == "Sampler" )
		{
			int			type = ( ParseXmlAttribute( *materialDefinition , "type" , 0 ) % eSamplerType::SAMPLER_TOTAL );
			int			bindSlot = ParseXmlAttribute( *materialDefinition , "bindSlot" , 0 );

			Sampler* temp = new Sampler( g_theRenderer , ( eSamplerType ) type );
			m_samplersPerSlot[ bindSlot ] = temp;
		}
		materialDefinition = materialDefinition->NextSiblingElement();
	}
}


void Material::SetData( void const* data , size_t dataSize )
{
	m_uboCPUData.resize( dataSize );
	memcpy( &m_uboCPUData[ 0 ] , data , dataSize );
	m_uboIsDirty = true;

	if ( nullptr == m_ubo )
	{
		m_ubo = new RenderBuffer( g_theRenderer , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	m_ubo->Update( data , dataSize , dataSize );
}

void Material::SetShader( Shader* const shader )
{
	m_shaderState->SetCurrentShader( shader );
}
