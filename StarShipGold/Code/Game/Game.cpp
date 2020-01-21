#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Bullet.hpp"
#include "Game/Astroid.hpp"
#include "Game/Bettle.hpp"
#include "Game/Wasp.hpp"
#include "Game/Debris.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // must #include Windows.h before #including Xinput.h
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility



Game::Game()
{
	playerShip =new PlayerShip(this,Vec2(WORLD_CENTER_X,WORLD_CENTER_Y));
	playerShip->m_isDead=true;

	rn= RandomNumberGenerator();
	
	
}

void Game::Update( float deltaseconds )
{
	VibrateJoystick(deltaseconds);
	playerShip->Update(deltaseconds);

	if( lives<=0 )
	{
		timeTowait-=deltaseconds;
		if(timeTowait<=0 )
		{ 
		attractMode=true;
		}
	}

	if( attractMode )
	{ 
		return;
	}



	CamerShake(m_intensity);
	

	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if( bullets[index]!= nullptr )
		{
			bullets[index]->Update(deltaseconds);
		}
	}

	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr )
		{
			astroids[index]->Update(deltaseconds);
		}
	}

	for( int index=0; index<MAX_DEBRIS; index++ )
	{
		if( debris[index]!= nullptr )
		{
			debris[index]->Update(deltaseconds);
		}
	}

	for( int index=0; index<MAX_BUGS; index++ )
	{
		if( bettle[index]!=nullptr )
		{
			bettle[index]->Update(deltaseconds);
		}

		if( wasp[index]!=nullptr )
		{
			wasp[index]->Update( deltaseconds );
		}
	}
	
	HandelKeyEvents();

	if( GetAliveEnititesCount()==0 )
	{
		SpawnWaves();
	}

	DetectCollissionsBtweenAstroidsAndBullets();
	DetectCollissionBtweenBugsAndBullets();
	DetectCollissionBtweenShipAndAstroid();
	DetectCollissionBtweenShipAndBugs();

	 
	
	if(isDebugging )
	{ 
		DrawVelocityLine();
		DrawRingsForAllEnitiesAlive();
		DrawLinesFromShipToAllEntitiesAlive();
	}
	
	FindAndMakeGarbageEntities();
	DeleteGarbageEntities();
	

	
	
}

void Game::Render()
{
	if( attractMode )
	{
		DisplayAttractScreen();
		return;
	}

	if(!playerShip->m_isDead )
	{ 
	playerShip->Render();
	}


	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if( bullets[index]!= nullptr )
		{
			bullets[index]->Render( );
		}
	}

	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr )
		{
			astroids[index]->Render( );
		}
	}

	for( int index=0; index<MAX_DEBRIS; index++ )
	{
		if( debris[index]!= nullptr )
		{
			debris[index]->Render( );
		}
	}

	for( int index=0; index<MAX_BUGS; index++ )
	{
		if( bettle[index]!=nullptr )
		{
			bettle[index]->Render( );
		}

		if( wasp[index]!=nullptr )
		{
			wasp[index]->Render( );
		}
	}
}

void Game::SpawnBullet( Vec2 postiton, float orientationDegrees )
{
	
	if( GetBulletCount()==MAX_BULLETS )
	{
		ERROR_RECOVERABLE("Max Bullets reached");
		return;
	}

	for( int index=0; index<MAX_BULLETS; index++ )
	{
		
		if( bullets[index]==nullptr && canShoot )
		{
			Bullet* b= new Bullet(this,postiton);
			b->m_position=b->m_position+Vec2(2.5f*CosDegrees(orientationDegrees),2.5f*SinDegrees(orientationDegrees));
			bullets[index]=b;
			b->m_orientationDegrees=orientationDegrees;
			canShoot=false;
			break;
		}
		
	}

	
	
}

void Game::SpawnBulletJoyStick( Vec2 position, float orientationDegress )
{
	for( int index=0; index<MAX_BULLETS; index++ )
	{

		if( bullets[index]==nullptr )
		{
			Bullet* b= new Bullet( this, position );
			b->m_position=b->m_position+Vec2( 2.5f*CosDegrees( orientationDegress ), 2.5f*SinDegrees( orientationDegress ) );
			bullets[index]=b;
			b->m_orientationDegrees=orientationDegress;
			break;
		}
	}
}

void Game::SpawnInitialAstroids()
{
	//RandomNumberGenerator random= RandomNumberGenerator();

	for( int index=0; index<5; index++ )
	{
		int numberX=rn.GetRandomIntInRange( 0, 200 );
		int numberY=rn.GetRandomIntInRange( 0, 100 );
		Astroid* a= new Astroid( this, Vec2( (float)numberX, (float)numberY ) );
		astroids[index]=a;
	}
}

void Game::ResetShip()
{
	if( playerShip->m_isDead && lives>0)
	{
		playerShip->m_position=Vec2(WORLD_CENTER_X,WORLD_CENTER_Y);
		playerShip->m_velocity=Vec2(0.f,0.f);
		playerShip->m_isDead=false;
		//lives--;
	}

	
}

void Game::SpawnAstroidFromKeyboard()
{
	if( GetAstroidCount()==MAX_ASTEROIDS )
	{
		ERROR_RECOVERABLE("Max astroid limit");
		return;
	}

	//RandomNumberGenerator random= RandomNumberGenerator();
	int numberX=rn.GetRandomIntInRange(0,200);
	int numberY=rn.GetRandomIntInRange(0,100);


	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]==nullptr && canSpawnAstroid )
		{
			canSpawnAstroid=false;
			Astroid* a= new Astroid(this,Vec2((float)numberX,(float)numberY));
			astroids[index]=a;
			break;
		}
	}
}

void Game::SpawnAstroid()
{
	int numberX=rn.GetRandomIntInRange( 0, 200 );
	int numberY=rn.GetRandomIntInRange( 0, 100 );


	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]==nullptr )
		{
			Astroid* a= new Astroid( this, Vec2( (float)numberX, (float)numberY ) );
			astroids[index]=a;
			break;
		}
	}
}

bool Game::IsOffscreen(const  Entity* entity )
{
	if( entity->m_position.x - entity->m_cosmeticRadius >200.f || entity->m_position.x + entity->m_cosmeticRadius< 0.f 
		|| entity->m_position.y-entity->m_cosmeticRadius > 100.f || entity->m_position.y+ entity->m_cosmeticRadius < 0.f )
	{
		return true;
	}
	return false;
}

void Game::FindAndMakeGarbageEntities()
{
	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if( bullets[index]!=nullptr )
		{
			if( IsOffscreen( bullets[index] ) )
			{
				bullets[index]->m_isGarbage=true;
			}
		}
	}

	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr )
		{
			if( IsOffscreen( astroids[index] ) )
			{
				astroids[index]->m_isGarbage=true;
			}
		}
	}

}

void Game::DeleteGarbageEntities()
{
	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if(bullets[index]!=nullptr )
		{ 
			if( bullets[index]->m_isGarbage )
			{
				delete bullets[index];
				bullets[index]=nullptr;
			}
		}
	}

	for( int index =0; index<MAX_ASTEROIDS; index++ )
	{
		if(astroids[index]!=nullptr )
		{ 
			if( astroids[index]->m_isGarbage )
			{
				delete astroids[index];
				astroids[index]=nullptr;
			}
		}
	}

	for( int index=0; index<MAX_DEBRIS; index++ )
	{
		if( debris[index]!=nullptr )
		{
			if( debris[index]->m_isGarbage )
			{
				delete debris[index];
				debris[index]=nullptr;
			}
		}
	}

	for( int index=0; index<MAX_BUGS; index++ )
	{
		if( bettle[index]!=nullptr )
		{
			if( bettle[index]->m_isGarbage )
			{
				delete bettle[index];
				bettle[index]=nullptr;
			}
		}

		if( wasp[index]!=nullptr )
		{
			if( wasp[index]->m_isGarbage )
			{
				delete wasp[index];
				wasp[index]=nullptr;
			}
		}
	}
}

void Game::HandelKeyEvents()
{
	if(g_theapp->oPressed )
	{ 
		SpawnAstroidFromKeyboard();
	}

	
}

bool Game::DoEntitiesOverlap( const Entity* a, const Entity* b ) const
{
	return DoDiscsOverlap(a->m_position,a->m_physicsRadius,b->m_position,b->m_physicsRadius);
}

void Game::DetectCollissionsBtweenAstroidsAndBullets()
{
	//RandomNumberGenerator random= RandomNumberGenerator();
	int num_debrisTospawn=0;

	for( int bulletIndex=0; bulletIndex<MAX_BULLETS; bulletIndex++ )
	{
		if( bullets[bulletIndex]!=nullptr && bullets[bulletIndex]->m_isGarbage!=true )
		{
			for( int astroidIndex=0; astroidIndex<MAX_ASTEROIDS; astroidIndex++ )
			{
				if( astroids[astroidIndex]!=nullptr && astroids[astroidIndex]->m_isGarbage!=true )
				{
					if( DoEntitiesOverlap( bullets[bulletIndex], astroids[astroidIndex] ) )
					{
						astroids[astroidIndex]->m_health--;

						bullets[bulletIndex]->m_isGarbage=true;
						num_debrisTospawn=rn.GetRandomIntInRange(3,5);
						SpawnDebris(num_debrisTospawn,Rgba8(255,0,0,128),bullets[bulletIndex]->m_position);

						if( astroids[astroidIndex]->m_health<=0 )
						{
							num_debrisTospawn=rn.GetRandomIntInRange( 6, 10 );
							SpawnDebris( num_debrisTospawn, Rgba8( 100, 100, 100, 128 ), bullets[bulletIndex]->m_position );
							astroids[astroidIndex]->m_isGarbage=true;
							m_intensity=.5f;
						}
					}
				}
			}
		}
	}
}



void Game::DetectCollissionBtweenBugsAndBullets()
{
	//RandomNumberGenerator random= RandomNumberGenerator();
	int num_debrisTospawn=0;

	for( int bulletIndex=0; bulletIndex<MAX_BULLETS; bulletIndex++ )
	{
		if( bullets[bulletIndex]!=nullptr && bullets[bulletIndex]->m_isGarbage!=true )
		{
			for( int bugsIndex=0; bugsIndex<MAX_BUGS; bugsIndex++ )
			{
				if( bettle[bugsIndex]!=nullptr && bettle[bugsIndex]->m_isGarbage!=true )
				{
					if( DoEntitiesOverlap( bullets[bulletIndex], bettle[bugsIndex] ) )
					{
						bettle[bugsIndex]->m_health--;

						bullets[bulletIndex]->m_isGarbage=true;
						num_debrisTospawn=rn.GetRandomIntInRange( 3, 5 );
						SpawnDebris( num_debrisTospawn, Rgba8( 255, 0, 0, 128 ), bullets[bulletIndex]->m_position );

						if( bettle[bugsIndex]->m_health<=0 )
						{
							num_debrisTospawn=rn.GetRandomIntInRange( 6, 10 );
							SpawnDebris( num_debrisTospawn, Rgba8( 60, 50, 100, 128 ), bullets[bulletIndex]->m_position );
							bettle[bugsIndex]->Die();
							m_intensity=.5f;
						}
					}
				}

				if( wasp[bugsIndex]!=nullptr && wasp[bugsIndex]->m_isGarbage!=true )
				{
					if( DoEntitiesOverlap( bullets[bulletIndex], wasp[bugsIndex] ) )
					{
						wasp[bugsIndex]->m_health--;

						bullets[bulletIndex]->m_isGarbage=true;
						num_debrisTospawn=rn.GetRandomIntInRange( 3, 5 );
						SpawnDebris( num_debrisTospawn, Rgba8( 255, 0, 0, 128 ), bullets[bulletIndex]->m_position );

						if( wasp[bugsIndex]->m_health<=0 )
						{
							num_debrisTospawn=rn.GetRandomIntInRange( 6, 10 );
							SpawnDebris( num_debrisTospawn, Rgba8( 0, 0, 150, 128 ), bullets[bulletIndex]->m_position );
							wasp[bugsIndex]->Die();
							m_intensity=.5f;
						}
					}
				}
			}
		}
	}
}

void Game::DetectCollissionBtweenShipAndAstroid()
{
	int num_debrisTospawn=0;
	if(!playerShip->m_isDead)
	{ 
		for( int index=0; index<MAX_ASTEROIDS; index++ )
		{
			if( astroids[index]!=nullptr )
			{
				if( DoEntitiesOverlap( playerShip, astroids[index] ) )
				{
					astroids[index]->m_health--;
					num_debrisTospawn=rn.GetRandomIntInRange( 15, 20 );
					SpawnDebris( num_debrisTospawn, Rgba8( 132, 156, 169, 128 ), playerShip->m_position );
					m_intensity=1.f;
					vibration_intensity=1.f;
					playerShip->m_isDead=true;
					lives--;
				}
			}
		}
	}
}



void Game::DetectCollissionBtweenShipAndBugs()
{
	int num_debrisTospawn=0;
	if(!playerShip->m_isDead )
	{ 
		for( int index=0; index<MAX_BUGS; index++ )
		{
			if( bettle[index]!= nullptr )
			{
				if( DoEntitiesOverlap( bettle[index], playerShip ) )
				{
					
					num_debrisTospawn=rn.GetRandomIntInRange( 15, 20 );
					SpawnDebris( num_debrisTospawn, Rgba8( 132, 156, 169, 128 ), playerShip->m_position );
					m_intensity=1.f;
					vibration_intensity=1.f;
					playerShip->m_isDead=true;
					lives--;
				}
			}

			if( wasp[index]!=nullptr )
			{
				if( DoEntitiesOverlap( wasp[index], playerShip ) )
				{
					
					num_debrisTospawn=rn.GetRandomIntInRange( 15, 20 );
					SpawnDebris( num_debrisTospawn, Rgba8( 132, 156, 169, 128 ), playerShip->m_position );
					m_intensity=1.f;
					vibration_intensity=1.f;
					playerShip->m_isDead=true;
					lives--;
				}
			}
		}
	}
}

void Game::DrawLine( const Vec2 &start,const Vec2 &end, const Rgba8& color, float thickness )
{
	

	Vec2 fwd=end-start;
	fwd.SetLength(thickness*0.5f);
	Vec2 lef=fwd.GetRotated90Degrees();

	Vec2 endRight= end+fwd-lef;
	Vec2 startRight= start-fwd-lef;
	Vec2 endLeft= end+fwd+lef;
	Vec2 startLeft= start-fwd+lef;

	Vec3 er=Vec3(endRight.x,endRight.y,0.f);
	Vec3 el=Vec3( endLeft.x, endLeft.y, 0.f );
	Vec3 sr=Vec3( startRight.x, startRight.y, 0.f );
	Vec3 sl=Vec3( startLeft.x, startLeft.y, 0.f );




	Vertex_PCU vert1[3] = 
	{
		Vertex_PCU(sr,color,Vec2(0.f,0.f)),
		Vertex_PCU(er,color,Vec2(0.f,0.f)),
		Vertex_PCU(el,color,Vec2(0.f,0.f))
		
	};

	Vertex_PCU vert2[3] =
	{
		Vertex_PCU( sr,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( el,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( sl,color,Vec2( 0.f,0.f ) )
	};

	render->DrawVertexArray(3,vert1);
	render->DrawVertexArray(3,vert2);
	

}

void Game::DrawRings( const Entity* entity )
{
	constexpr float theta= 360.f/16.f;

	float rCosmetic=entity->m_cosmeticRadius;
	float rPhysics=entity->m_physicsRadius;

	Vec2 center=entity->m_position;

	Rgba8 colorCosmetic=Rgba8(255,0,255,255);
	Rgba8 colorPhysics=Rgba8(0,255,255,255);

	for( int index=0; index<16; index++ )
	{
		float degree1=theta*float(index);
		float degree2=theta*float(index+1);

		DrawLine(center+Vec2(rCosmetic*CosDegrees(degree1),rCosmetic*SinDegrees(degree1)),center+Vec2(rCosmetic*CosDegrees(degree2),rCosmetic*SinDegrees(degree2)),colorCosmetic,.2f);
	}

	for( int index=0; index<16; index++ )
	{
		float degree1=theta*float( index );
		float degree2=theta*float( index+1 );

		DrawLine( center+Vec2( rPhysics*CosDegrees( degree1 ), rPhysics*SinDegrees( degree1 ) ), center+Vec2( rPhysics*CosDegrees( degree2 ), rPhysics*SinDegrees( degree2 ) ), colorPhysics, .2f );
	}
}

void Game::DrawRingsForAllEnitiesAlive()
{
	if( playerShip->m_isDead==false )
	{
		DrawRings(playerShip);
	}

	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if( bullets[index]!=nullptr )
		{
			DrawRings(bullets[index]);
		}
	}

	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr )
		{
			DrawRings(astroids[index]);
		}
	}
}

void Game::DrawLinesFromShipToAllEntitiesAlive()
{
	Rgba8 lineColor=Rgba8(50,50,50,255);

	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if( bullets[index]!=nullptr && bullets[index]->m_isGarbage==false )
		{
			DrawLine(playerShip->m_position,bullets[index]->m_position,lineColor,0.2f);
		}
	}

	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr && astroids[index]->m_isGarbage==false )
		{
			DrawLine(playerShip->m_position,astroids[index]->m_position,lineColor,0.2f);
		}
	}
}

void Game::DrawVelocityLine()
{
	Rgba8 linColor= Rgba8(255,255,0,255);

	DrawLine(playerShip->m_position,(playerShip->m_position+playerShip->m_velocity),linColor,.3f);

	
	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if( bullets[index]!=nullptr && bullets[index]->m_isGarbage==false )
		{
			DrawLine( bullets[index]->m_position,bullets[index]->m_velocity+bullets[index]->m_position, linColor, 0.2f );
		}
	}

	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr && astroids[index]->m_isGarbage==false )
		{
			DrawLine( astroids[index]->m_position,astroids[index]->m_position+astroids[index]->m_velocity*10.f, linColor, 0.2f );
		}
	}
}

void Game::ToggleDebugging()
{
		if( g_theapp->f1Presssed )
		{
			isDebugging=!isDebugging;
		}
}

int Game::GetBulletCount()
{
	int count=0;
	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if( bullets[index]!=nullptr )
		{
			count++;
		}
	}
	return count;
}

int Game::GetAstroidCount()
{
	int count=0;
	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr )
		{
			count++;
		}
	}
	return count;
}

Vec2 Game::GetPlayerShipPosition()
{
	return playerShip->m_position;
}

void Game::SpawnWaves()
{
	int base_numberOfAstroids=5;
	int base_numberOfBettles=0;
	int base_numberOfWasps=0;


	if( m_currentlevel<=max_levles)
	{
		int num_astroids = base_numberOfAstroids+(m_currentlevel+1);
		int num_bettles =  base_numberOfBettles+(m_currentlevel+1);
		int num_wasps = base_numberOfWasps+(m_currentlevel+1);

		m_currentlevel++;

   		for( int index=0; index<num_astroids; index++ )
		{
			SpawnAstroid();
		}

		for( int index=0; index<num_bettles; index++ )
		{
			SpawnBettle();
		}

		for( int index=0; index< num_wasps; index++ )
		{
			SpawnWasp();
		}
	}
}

int Game::GetAliveEnititesCount()
{
	int count=0;
	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr )
		{
			count++;
		}
	}

	for( int index=0; index< MAX_BUGS; index++ )
	{
		if( bettle[index]!= nullptr )
		{
			count++;
		}

		if( wasp[index]!=nullptr )
		{
			count++;
		}
	}
	return count;
}

void Game::SpawnDebris( int numberOfDebris, Rgba8 debris_color, Vec2 position )
{
	//ndomNumberGenerator random= RandomNumberGenerator();
	float degreesFraction=360.f/(float)numberOfDebris;
	float degressToRotate=0;

	for( int index=0; index<numberOfDebris; index++ )
	{
		float offsetx=(float)rn.GetRandomIntInRange(0,5); 
		float offsety= (float)rn.GetRandomIntInRange(0,5);
		debris[index]=new Debris(this,Vec2(position.x+offsetx,position.y+offsety),debris_color);
		debris[index]->m_orientationDegrees=degressToRotate;

		degressToRotate+=degreesFraction;
	}
}

void Game::CamerShake( float intensity )
{
	//RandomNumberGenerator rng= RandomNumberGenerator();
	m_intensity-=0.01f;
	m_intensity=ClampZeroToOne(m_intensity);
	float maxShakeDist = intensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast<float> (rn.GetRandomIntInRange( (int)-maxShakeDist, (int)maxShakeDist ));
	float cameraShakeY = static_cast<float> (rn.GetRandomIntInRange( (int)-maxShakeDist, (int)maxShakeDist ));

	g_theapp->game_cam->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( 200.f, 100.f ) );
	g_theapp->game_cam->Translate2D( Vec2( cameraShakeX, cameraShakeY ) );

	
}

void Game::SpawnBettle()
{
	
	Vec2 pos=GetRandomLocationJustOffScreen();

	for( int index=0; index<MAX_BUGS; index++ )
	{
		if( bettle[index]== nullptr )
		{
			bettle[index]= new Bettle(this,pos);
			break;
		}
	}
}

void Game::SpawnWasp()
{
	
	Vec2 pos=GetRandomLocationJustOffScreen();

	for( int index=0; index<MAX_BUGS; index++ )
	{
		if( wasp[index]== nullptr )
		{
			wasp[index]= new Wasp( this, pos );
			break;
		}
	}
}

bool Game::GetPlayerShipStatus()
{
	return playerShip->m_isDead;
}

void Game::RenderUI()
{
	
	render->BeginCamera(*(g_theapp->ui_cam));

	float offset=50.f;
	Vertex_PCU iconVertices[15];
	Vertex_PCU iconVertices_original[15];
	Rgba8 iconColor = Rgba8( 132, 0, 0, 128 );

	iconVertices[0] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[1] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[2] = Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) ); 
	iconVertices[3] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[4] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[5] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );

	iconVertices[6] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[7] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[8] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );

	iconVertices[9] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[10] = Vertex_PCU( Vec3( 2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[11] = Vertex_PCU( Vec3( 0.f, 2.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );

	iconVertices[12] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[13] = Vertex_PCU( Vec3( 2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[14] = Vertex_PCU( Vec3( 0.f, -2.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
				
	iconVertices_original[0] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[1] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[2] = Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) ); 
				
	iconVertices_original[3] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[4] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[5] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
				
	iconVertices_original[6] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[7] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[8] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
				
	iconVertices_original[9] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[10] = Vertex_PCU( Vec3( 2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[11] = Vertex_PCU( Vec3( 0.f, 2.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
				
	iconVertices_original[12] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[13] = Vertex_PCU( Vec3( 2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices_original[14] = Vertex_PCU( Vec3( 0.f, -2.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );

	for(int livesIndex=0; livesIndex<lives; livesIndex++ )
	{ 
	for( int index=0; index<15; index++ )
	{
		iconVertices[index].m_position=TransformPosition3DXY( iconVertices_original[index].m_position, 9.f, 0.f, Vec2(30.f+(livesIndex*offset),UI_YMaX-30.f ));
	}
	render->BindTexture( nullptr );
	render->DrawVertexArray(15,iconVertices);
	}


}

void Game::DisplayAttractScreen()
{
	Vertex_PCU iconVertices[15];
	Vertex_PCU iconVertices_original[15];


	Rgba8 iconColor = Rgba8( 11, 0, 132, 128 );

	iconVertices[0] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[1] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[2] = Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[3] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[4] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[5] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );

	iconVertices[6] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[7] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[8] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );

	iconVertices[9] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[10] = Vertex_PCU( Vec3( 2.f, 1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[11] = Vertex_PCU( Vec3( 0.f, 2.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );

	iconVertices[12] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[13] = Vertex_PCU( Vec3( 2.f, -1.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );
	iconVertices[14] = Vertex_PCU( Vec3( 0.f, -2.f, 0.f ), iconColor, Vec2( 0.f, 0.f ) );

	for( int index=0; index<15; index++ )
	{
		iconVertices[index].m_position=TransformPosition3DXY( iconVertices[index].m_position, 9.f, 0.f, Vec2( 100.f, 50.f ) );
	}
	render->DrawVertexArray(15,iconVertices);
}

void Game::DeleteAllAliveEntities()
{
	for( int index=0; index<MAX_ASTEROIDS; index++ )
	{
		if( astroids[index]!=nullptr )
		{
			delete astroids[index];
			astroids[index]=nullptr;
		}
	}  

	for( int index=0; index<MAX_BULLETS; index++ )
	{
		if( bullets[index]!=nullptr )
		{
			delete bullets[index];
			bullets[index]=nullptr;
		}
	}

	for( int index=0; index<MAX_BUGS; index++ )
	{
		if( bettle[index]!=nullptr )
		{
			delete bettle[index];
			bettle[index]=nullptr;
		}

		if( wasp[index]!=nullptr )
		{
			delete wasp[index];
			wasp[index]=nullptr;
		}
	}
}

void Game::HandleStateEvents()
{
	if(attractMode == true && playerShip->m_isDead && lives>0 )
	{ 
	attractMode=false;
	lives--;
	playerShip->m_isDead=false;
	}
	
	if( attractMode == false && playerShip->m_isDead==true )
	{
		ResetShip();
	}

	if( attractMode == true && playerShip->m_isDead && lives<=0 )
	{
		lives=4;
		timeTowait=3.f;
		m_currentlevel=0;
		attractMode=false;
		lives--;
		playerShip->m_isDead=false;
		playerShip->m_position=Vec2(WORLD_CENTER_X,WORLD_CENTER_Y);
		playerShip->m_velocity=Vec2(0.f,0.f);
		DeleteAllAliveEntities();
		ResetShip();
	}

}

Vec2 Game::GetRandomLocationJustOffScreen()
{
	for( ;;)
	{
		float x=(float)rn.GetRandomIntInRange(-5,205);
		float y=(float)rn.GetRandomIntInRange(-5,105);

		if( x<0 || x> WORLD_SIZE_X || y<0 || y> WORLD_SIZE_Y )
		{
			return Vec2(x,y);
		}
	}
}

void Game::VibrateJoystick(float deltaseconds)
{
	
	const XboxController& controller= input->GetXboxController( 0 );

	if( !controller.IsCOnnected() )
	{
		return;
	}

	
	XINPUT_VIBRATION xboxviration;
	memset(&xboxviration,0,sizeof(xboxviration));

	if(vibration_intensity>0 )
	{ 
	xboxviration.wLeftMotorSpeed=(WORD)(1000.f*vibration_intensity);
	xboxviration.wRightMotorSpeed=(WORD)(1000.f*vibration_intensity);
	}

	if(vibration_intensity>0 )
	{ 
	vibration_intensity-=0.5f*deltaseconds;
	}

	XInputSetState(controller.GetControllerID(),&xboxviration);

}

