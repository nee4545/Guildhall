#include "Game/AIController.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

AIController::AIController( Game* game )
{
	m_game = game;
	m_decisionTimer = new Timer();
	m_clock = new Clock();

	m_decisionTimer->SetSeconds( m_clock , 4.f );
}

void AIController::Update( float deltaSeconds )
{
	if ( m_decisionTimer->HasElapsed() )
	{
		MakeDecision();
		ExecuteDecision();
		m_decisionTimer->Reset();
	}
}

void AIController::MakeDecision()
{
	int randomNumber = m_game->m_rng.RollRandomIntInRange( 0 , 22 );

	if ( randomNumber >= 0 && randomNumber <= 7 )
	{
		m_decision = SPAWN_MELEE_HERO;
		return;
	}

	else if ( randomNumber >= 8 && randomNumber <= 13 )
	{
		m_decision = SPAWN_RANGE_HERO;
		return;
	}

	else if(randomNumber >=14 && randomNumber <=16 )
	{
		randomNumber = m_game->m_rng.RollRandomIntInRange( 1 , 4 );

		if ( randomNumber <= 3 )
		{
			if ( m_game->m_enemyUpgradeState.meleeHeroState != UPGRADE_LARGE )
			{
				m_decision = UPGRADE_MELEE_HERO;
			}
			else
				m_decision = SPAWN_MELEE_HERO;
		}
		else
		{
			if ( m_game->m_enemyUpgradeState.rangeHeroState != UPGRADE_LARGE )
			{
				m_decision = UPGRADE_RANGE_HERO;
			}
			else
				m_decision = SPAWN_RANGE_HERO;
		}
	}
	else if( randomNumber >= 17 && randomNumber <= 20  )
	{
		if ( randomNumber >= 17 && randomNumber <=18 )
		{
			if ( m_game->m_enemyTower1 == nullptr || m_game->m_enemyTower2 == nullptr )
			{
				m_decision = SPAWN_TOWER;
			}
			else
				m_decision = SPAWN_MELEE_HERO;
		}
		if ( randomNumber >= 19  && randomNumber <=20 )
		{
			if ( m_game->m_enemyUpgradeState.towerState != UPGRADE_LARGE )
			{
				m_decision = UPGRADE_TOWER;
			}
			else
				m_decision = SPAWN_MELEE_HERO;
		}
	}
	else
	{
		if ( randomNumber == 21 )
		{
			m_decision = SPAWN_TOWERDESTROYER;
		}
		else
			m_decision = UPGRADE_TOWER_DESTROYER;
	}


}

void AIController::ExecuteDecision()
{
	if ( m_decision == SPAWN_MELEE_HERO )
	{
		int randomNumber = m_game->m_rng.RollRandomIntInRange( 1 , 4 );

		if ( randomNumber == 1 )
		{
			m_game->CreateMeleeHero( FACTION_BAD , m_game->m_enemyUpgradeState.meleeHeroState , LANE1_1 );
		}

		if ( randomNumber == 2 )
		{
			m_game->CreateMeleeHero( FACTION_BAD , m_game->m_enemyUpgradeState.meleeHeroState , LANE1_2 );
		}

		if ( randomNumber == 3 )
		{
			m_game->CreateMeleeHero( FACTION_BAD , m_game->m_enemyUpgradeState.meleeHeroState , LANE2_1 );
		}

		if ( randomNumber == 4 )
		{
			m_game->CreateMeleeHero( FACTION_BAD , m_game->m_enemyUpgradeState.meleeHeroState , LANE2_2 );
		}

		return;
	}

	if ( m_decision == SPAWN_RANGE_HERO )
	{
		int randomNumber = m_game->m_rng.RollRandomIntInRange( 1 , 4 );

		if ( randomNumber == 1 )
		{
			m_game->CreateRangeHero( FACTION_BAD , m_game->m_enemyUpgradeState.rangeHeroState , LANE1_1 );
		}

		if ( randomNumber == 2 )
		{
			m_game->CreateRangeHero( FACTION_BAD , m_game->m_enemyUpgradeState.rangeHeroState , LANE1_2 );
		}

		if ( randomNumber == 3 )
		{
			m_game->CreateRangeHero( FACTION_BAD , m_game->m_enemyUpgradeState.rangeHeroState , LANE2_1 );
		}

		if ( randomNumber == 4 )
		{
			m_game->CreateRangeHero( FACTION_BAD , m_game->m_enemyUpgradeState.rangeHeroState , LANE2_2 );
		}

		return;
	}

	if ( m_decision == SPAWN_TOWER )
	{
		if ( m_game->m_enemyTower1 == nullptr )
		{
			m_game->SpawnTower( Vec2() , FACTION_BAD , LANE1_1 , m_game->m_enemyUpgradeState.towerState );
		}
		else if ( m_game->m_enemyTower2 == nullptr )
		{
			m_game->SpawnTower( Vec2() , FACTION_BAD , LANE1_2 , m_game->m_enemyUpgradeState.towerState );
		}
	}

	if ( m_decision == SPAWN_TOWERDESTROYER )
	{
		if ( m_game->m_playerTower1 )
		{
			m_game->SpawnTowerDestroyer( FACTION_BAD , m_game->m_enemyUpgradeState.towerDestroyerState , LANE1_1 );
		}
		else if ( m_game->m_playerTower2 )
		{
			m_game->SpawnTowerDestroyer( FACTION_BAD , m_game->m_enemyUpgradeState.towerDestroyerState , LANE1_2 );
		}
		else
		{
			int randomNum = m_game->m_rng.RollRandomIntInRange( 0 , 1 );

			if ( randomNum == 0 )
			{
				m_game->SpawnTowerDestroyer( FACTION_BAD , m_game->m_enemyUpgradeState.towerDestroyerState , LANE1_1 );
			}
			else
			{
				m_game->SpawnTowerDestroyer( FACTION_BAD , m_game->m_enemyUpgradeState.towerDestroyerState , LANE1_2 );
			}

		}
	}

	if ( m_decision == UPGRADE_RANGE_HERO )
	{
		switch ( m_game->m_enemyUpgradeState.rangeHeroState )
		{
		case UPGRADE_NONE:
		{
			m_game->m_enemyUpgradeState.rangeHeroState = UPGRADE_SMALL;
			break;
		}
		case UPGRADE_SMALL:
		{
			m_game->m_enemyUpgradeState.rangeHeroState = UPGRADE_MEDIUM;
			break;
		}
		case UPGRADE_MEDIUM:
		{
			m_game->m_enemyUpgradeState.rangeHeroState = UPGRADE_LARGE;
			break;
		}
		default:break;
		}

		return;
	}

	if ( m_decision == UPGRADE_MELEE_HERO )
	{
		switch ( m_game->m_enemyUpgradeState.meleeHeroState )
		{
		case UPGRADE_NONE:
		{
			m_game->m_enemyUpgradeState.meleeHeroState = UPGRADE_SMALL;
			break;
		}
		case UPGRADE_SMALL:
		{
			m_game->m_enemyUpgradeState.meleeHeroState = UPGRADE_MEDIUM;
			break;
		}
		case UPGRADE_MEDIUM:
		{
			m_game->m_enemyUpgradeState.meleeHeroState = UPGRADE_LARGE;
			break;
		}
		default:break;
		}
		return;
	}

	if ( m_decision == UPGRADE_TOWER )
	{
		switch ( m_game->m_enemyUpgradeState.towerState )
		{
		case UPGRADE_NONE:
		{
			m_game->m_enemyUpgradeState.towerState = UPGRADE_SMALL;
			break;
		}
		case UPGRADE_SMALL:
		{
			m_game->m_enemyUpgradeState.towerState = UPGRADE_MEDIUM;
			break;
		}
		case UPGRADE_MEDIUM:
		{
			m_game->m_enemyUpgradeState.towerState = UPGRADE_LARGE;
			break;
		}
		default:break;
		}
		return;
	}

	if ( m_decision == UPGRADE_TOWER_DESTROYER )
	{
		switch ( m_game->m_enemyUpgradeState.towerDestroyerState )
		{
		case UPGRADE_NONE:
		{
			m_game->m_enemyUpgradeState.towerDestroyerState = UPGRADE_SMALL;
			break;
		}
		default:break;
		}
		return;
	}

	


}
