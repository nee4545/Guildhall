#include "Game/PotentialFieldCreator.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PotentialFields.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/ThirdParty/ImGuiSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"

extern ImGuiSystem* g_theGUI;


PotentialFieldCreator::PotentialFieldCreator()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2(0.f,0.f) , Vec2(160.f,90.f) );

	m_spriteTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CommandoItems/cannonTop.png" );

	Vec2 vec = Vec2( 1.f , 0.f );
	//float ang =vec.GetAngleDegrees();
}

PotentialFieldCreator::~PotentialFieldCreator()
{
	
}

void PotentialFieldCreator::LoadPotentialFieldFromXml()
{
	m_potentialField = new PotentialField(4.f,10.f,Vec2());

	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/GameConfig/PotentialFieldData.xml" );

	m_potentialField->nodes.clear();

	tinyxml2::XMLElement* root = doc.FirstChildElement( "PotentialFieldNodes" );

	for ( tinyxml2::XMLElement* ele = root->FirstChildElement( "Node" ); ele != nullptr; ele = ele->NextSiblingElement( "Node" ) )
	{
		PotentialFieldNode n;
		n.xCoord = ParseXmlAttribute( *ele , "xCoord" , -1 );
		n.yCoord = ParseXmlAttribute( *ele , "yCoord" , -1 );
		n.isConsiderd = ParseXmlAttribute( *ele , "isConsidered" , false );
		n.isCore = ParseXmlAttribute( *ele , "isCore" , false );
		n.direction = ParseXmlAttribute( *ele , "direction" , -1.f );

		m_potentialField->nodes.push_back( n );
	}

}

void PotentialFieldCreator::UpdateCameraMovement()
{
	m_camera->SetOrthoView( cameraPosition + ( Vec2( 0.f , 0.f ) / zoomFactor ) , cameraPosition + Vec2( 160.f , 90.f ) / zoomFactor );
}

void PotentialFieldCreator::Update()
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	m_mousePos.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	m_mousePos.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	ImGui::NewFrame();

	ImGui::Begin( "Potential Field Creator" );

	if ( ImGui::CollapsingHeader( "Sprite Controls" ) )
	{
		ImGui::SliderFloat( "Sprite Size" , &m_spriteSize , 5.f , 10.f );
		ImGui::SliderFloat( "Sector Radius" , &m_sectorRadius , 5.f , 30.f );
		ImGui::SliderFloat( "Sector Angle" , &m_sectorAngle , 30.f , 60.f );
		ImGui::SliderFloat( "Sprite Position X" , &m_spritePosition.x , 0.f , 80.f );
		ImGui::SliderFloat( " Sprite Position Y" , &m_spritePosition.y , 0.f , 45.f );
	}

	if ( ImGui::CollapsingHeader( "Camera Controls" ) )
	{
		ImGui::SliderFloat( "Camera Position X" , &cameraPosition.x , -100.f , 100.f );
		ImGui::SliderFloat( "Camera Position Y" , &cameraPosition.y , -100.f , 100.f );
		ImGui::SliderInt( "Zoom Factor" , &zoomFactor , 1 , 4 );
	}

	if ( ImGui::CollapsingHeader( "Potential Field Controls" ) )
	{
		ImGui::SliderInt( "Potential Field Size" , &m_potentialFieldRadius , 5 , 60 );
		ImGui::Text( "Press P to create potential Field" );
	}

	if ( ImGui::CollapsingHeader( "Potential Field Node" ) )
	{
		if ( currentChosenIndex > -1 )
		{
			ImGui::Text( "Current chosen node is shown in blue color" );
			ImGui::Checkbox( "Is considered" , &isConsidered );
			ImGui::Checkbox( "Is core" , &isCore );
			ImGui::SliderFloat( "Angle" , &angleDegrees , 0.f , 360.f );
			if ( ImGui::Button( "Update" ) )
			{
				UpdatePotentialFieldValues();
			}
			if ( ImGui::Button( "Save" ) )
			{
				Save();
			}
		}
	}

	ImGui::End();
	ImGui::EndFrame();

	UpdateCameraMovement();
	UpdateSprite();

	if ( g_theInput->WasKeyJustPressed( 'P' ) )
	{
		if ( m_potentialField == nullptr )
		{
			m_potentialField = new PotentialField( 5.f , m_potentialFieldRadius , m_spritePosition );
		}
		else
		{
			delete m_potentialField;
			m_potentialField = nullptr;

			m_potentialField = new PotentialField( 5.f , m_potentialFieldRadius , m_spritePosition );
		}
	}

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		if ( m_potentialField != nullptr )
		{
			currentChosenIndex = GetIndexInPotentialField( m_mousePos );
		}
	}
	
	if ( g_theInput->WasKeyJustPressed( 'X' ) )
	{
		LoadPotentialFieldFromXml();
	}

}

void PotentialFieldCreator::UpdateSprite()
{
	m_spriteAABB = AABB2( m_spritePosition.x - ( m_spriteSize * 0.5f ) , m_spritePosition.y - ( m_spriteSize * 0.5f ) , m_spritePosition.x + ( m_spriteSize * 0.5f ) , m_spritePosition.y + ( m_spriteSize * 0.5f ) );
}

void PotentialFieldCreator::Render()
{
	g_theRenderer->BeginCamera( *m_camera );
	g_theRenderer->BindTexture( m_spriteTex );
	g_theRenderer->DrawAABB2D( m_spriteAABB,Rgba8() );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawSector( m_spritePosition , m_forwardVector , m_sectorRadius , m_sectorAngle , Rgba8(100,0,0,100) );

	std::vector<Vertex_PCU> potVerts;
	if ( m_potentialField != nullptr )
	{
		for ( int i = 0; i < m_potentialField->nodes.size(); i++ )
		{
			AABB2 aabb = AABB2( m_potentialField->nodes[ i ].xCoord , m_potentialField->nodes[ i ].yCoord , m_potentialField->nodes[ i ].xCoord + 1 , m_potentialField->nodes[ i ].yCoord + 1 );
			AppendAABB2( potVerts , aabb , Rgba8( 100 , 100 , 100 , 100 ) );

		}
	}

	g_theRenderer->DrawVertexArray( potVerts );

	std::vector<Vertex_PCU> chosenVerts;
	if ( m_potentialField != nullptr )
	{
		if ( currentChosenIndex > -1 )
		{
			AABB2 aabb = AABB2( m_potentialField->nodes[ currentChosenIndex ].xCoord , m_potentialField->nodes[ currentChosenIndex ].yCoord , m_potentialField->nodes[ currentChosenIndex ].xCoord + 1 , m_potentialField->nodes[ currentChosenIndex ].yCoord + 1 );
			AppendAABB2( chosenVerts , aabb , Rgba8( 0 , 0 , 100 , 150 ) );
		}
	}


	if ( m_potentialField != nullptr )
	{
		for ( int i = 0; i < m_potentialField->nodes.size(); i++ )
		{
			if ( m_potentialField->nodes[ i ].isConsiderd )
			{
				if ( m_potentialField->nodes[ i ].direction >= 0.f )
				{
					Vec2 lineStartPos = Vec2(m_potentialField->nodes[ i ].xCoord + 0.45f,m_potentialField->nodes[i].yCoord +0.45f);
					Vec2 dirVec = lineStartPos.GetNormalized();
					Vec2 endPos = Vec2( lineStartPos.x + 0.45f , lineStartPos.y );
					Vec2 vec = endPos - lineStartPos;
					vec = vec.GetNormalized();
					vec.RotateDegrees( m_potentialField->nodes[ i ].direction );
					endPos = lineStartPos + (vec.GetNormalized()*0.45f);

					g_theRenderer->DrawLine( lineStartPos , endPos , Rgba8( 100 , 0 , 0 , 150 ) , 0.1f );
					g_theRenderer->DrawDisc( endPos , 0.1f , Rgba8( 100 , 0 , 0 , 150 ) );
				}
			}
		}
	}

	g_theRenderer->DrawVertexArray( chosenVerts );

	g_theGUI->Render();
	g_theRenderer->EndCamera( *m_camera );
}

void PotentialFieldCreator::Save()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/GameConfig/PotentialFieldData.xml" );

	doc.Clear();

	//int posx = -m_potentialField->m_startPosition.x;
	//int posy = -m_potentialField->m_startPosition.y;

	//m_potentialField->m_startPosition.x = 0;
	//m_potentialField->m_startPosition.y = 0;

	//for ( int i = 0; i < m_potentialField->nodes.size(); i++ )
	//{
	//	m_potentialField->nodes[ i ].xCoord -= posx;
	//	m_potentialField->nodes[ i ].yCoord -= posy;
	//}


	tinyxml2::XMLNode* pfNode = doc.NewElement("PotentialFieldNodes");

	doc.InsertFirstChild( pfNode );

	tinyxml2::XMLElement* e = doc.NewElement( "Position" );
	e->SetAttribute( "posX" , m_potentialField->m_startPosition.x );
	e->SetAttribute( "posY" , m_potentialField->m_startPosition.y );

	pfNode->InsertEndChild( e );

	for ( int i = 0; i < m_potentialField->nodes.size(); i++ )
	{
		tinyxml2::XMLElement* ele = doc.NewElement( "Node" );
		ele->SetAttribute( "index" , i );
		ele->SetAttribute( "xCoord" , m_potentialField->nodes[ i ].xCoord );
		ele->SetAttribute( "yCoord" , m_potentialField->nodes[ i ].yCoord );
		ele->SetAttribute( "isConsidered" , m_potentialField->nodes[ i ].isConsiderd );
		ele->SetAttribute( "isCore" , m_potentialField->nodes[ i ].isCore );
		ele->SetAttribute( "direction" , m_potentialField->nodes[ i ].direction );

		pfNode->InsertEndChild( ele );
	}

	doc.SaveFile( "Data/GameConfig/PotentialFieldData.xml" );
}


void PotentialFieldCreator::UpdatePotentialFieldValues()
{
	m_potentialField->nodes[ currentChosenIndex ].isConsiderd = isConsidered;
	m_potentialField->nodes[ currentChosenIndex ].isCore = isCore;
	m_potentialField->nodes[ currentChosenIndex ].direction = angleDegrees;
}

int PotentialFieldCreator::GetIndexInPotentialField(Vec2 pos)
{
	IntVec2 coords = IntVec2( RoundDownToInt( pos.x ) , RoundDownToInt( pos.y ) );

	for( int i = 0; i < m_potentialField->nodes.size(); i++ )
	{
		if ( m_potentialField->nodes[i].xCoord == coords.x && m_potentialField->nodes[i].yCoord == coords.y )
		{
			return i;
		}
	}

	return -1;
}


