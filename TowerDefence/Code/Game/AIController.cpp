#include "Game/AIController.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

AIController::AIController()
{
	Clock* c = new Clock();
	m_timer = new Timer();
	
	m_rng = new RandomNumberGenerator();

	m_timer->SetSeconds( c , 1.f );
}

void AIController::TakeDefensiveDecision()
{
	int herosInLane1 = m_map->GetNumberOfHerosInLane( 1 );
	int herosInLane2 = m_map->GetNumberOfHerosInLane( 2 );
	int herosInLane3 = m_map->GetNumberOfHerosInLane( 3 );
	int herosInLane4 = m_map->GetNumberOfHerosInLane( 4 );

	int max = herosInLane1;
	int laneNum = 1;

	if ( max < herosInLane2 )
	{
		max = herosInLane2;
		laneNum = 2;
	}

	if ( max < herosInLane3 )
	{
		max = herosInLane3;
		laneNum = 3;
	}

	if ( max < herosInLane4 )
	{
		max = herosInLane4;
		laneNum = 4;
	}

	int randomNum = m_rng->RollRandomIntInRange( 0 , 1 );

	if ( randomNum == 0 )
	{
		m_decision.d = SPAWN_SPIKE;
		m_decision.xPosition = ( float )m_rng->RollRandomIntInRange( 30 , 45 );
		m_decision.laneNumber = laneNum;
	}

	if ( randomNum == 1 )
	{
		m_decision.d = SPAWN_HERO;
		m_decision.laneNumber = laneNum;
	}

}

void AIController::TakeOffensiveDecision()
{
	int randomNumber = m_rng->RollRandomIntInRange( 0 , 2 );
	int herosInLane1 = m_map->GetNumberOfHerosInLane( 1 );
	int herosInLane2 = m_map->GetNumberOfHerosInLane( 2 );
	int herosInLane3 = m_map->GetNumberOfHerosInLane( 3 );
	int herosInLane4 = m_map->GetNumberOfHerosInLane( 4 );

	int min = herosInLane1;
	int laneNum = 1;

	if ( min > herosInLane2 )
	{
		min = herosInLane2;
		laneNum = 2;
	}

	if ( min > herosInLane3 )
	{
		min = herosInLane3;
		laneNum = 3;
	}

	if ( min > herosInLane4 )
	{
		min = herosInLane4;
		laneNum = 4;
	}

	if ( randomNumber <= 1 )
	{
		if ( m_map->m_manager.enemyCoins >= 70 )
		{
			m_decision.d = SPAWN_DINO;
		}
		else
		{
			m_decision.d = SPAWN_HERO;
			m_decision.laneNumber = laneNum;
		}
	}
	else
	{
		m_decision.d = SPAWN_HERO;
		if ( m_decision.laneNumber != laneNum )
		{
			m_decision.laneNumber = laneNum;
		}
		else
		{
			while (int x= m_rng->RollRandomIntInRange( 1 , 4 ) != m_decision.laneNumber )
			{
				m_decision.laneNumber = x;
			}

			//m_decision.laneNumber = m_decision.laneNumber % 5;
		}
	}

}

void AIController::TakeStupidDecision()
{
	int randomNum = m_rng->RollRandomIntInRange( 0 , 5 );

	if ( randomNum <= 4 )
	{
		int n = m_rng->RollRandomIntInRange( 0 , 3 );
		int lane = m_rng->RollRandomIntInRange( 1 , 4 );


		if ( n == 0 )
		{
			m_decision.d = DO_NOTHING;
		}

		if ( n == 1 )
		{
			m_decision.d = SPAWN_HERO;
			m_decision.laneNumber = lane;
		}

		if ( n == 2 )
		{
			m_decision.d = SPAWN_SPIKE;
			m_decision.xPosition = ( float ) m_rng->RollRandomIntInRange( 30 , 45 );
			m_decision.laneNumber = lane;
		}

		if ( n == 3 )
		{
			if ( m_map->m_manager.enemyCoins >= 70 )
				m_decision.d = SPAWN_DINO;
			else
			{
				m_decision.d = SPAWN_SPIKE;
				m_decision.xPosition = ( float ) m_rng->RollRandomIntInRange( 30 , 45 );
				m_decision.laneNumber = lane;
			}
		}
	}
	else
	{
		int num = m_rng->RollRandomIntInRange( 0 , 1 );

		if ( num == 0 )
		{
			m_decision.d = DO_NOTHING;
		}
		else
		{
			if ( m_map->m_manager.enemyCoins > 70 )
			{
				m_decision.d = SPAWN_DINO;
			}
			else
			{
				m_decision.d = DO_NOTHING;
			}
		}
		
	}

}

void AIController::TakeDecision()
{
	int n = m_rng->RollRandomIntInRange( 1 , 3 );

	if ( n == 1 )
	{
		TakeStupidDecision();
	}

	if ( n == 2 )
	{
		TakeOffensiveDecision();
	}

	if ( n == 3 )
	{
		TakeDefensiveDecision();
	}
}

void AIController::Execute()
{
	Lanes lane;
	SelectedUnit unit;

	if ( m_decision.laneNumber == 1 )
	{
		lane = LANE_1;
	}
	if ( m_decision.laneNumber == 2 )
	{
		lane = LANE_2;
	}
	if ( m_decision.laneNumber == 3 )
	{
		lane = LANE_3;
	}
	if ( m_decision.laneNumber == 4 )
	{
		lane = LANE_4;
	}

	if ( m_decision.d == SPAWN_DINO )
	{
		unit = UNIT_DINO;
	}

	if ( m_decision.d == SPAWN_HERO )
	{
		unit = UNIT_HERO;
	}

	if ( m_decision.d == SPAWN_SPIKE )
	{
		unit = UNIT_SPIKE;
	}

	switch ( m_decision.d )
	{
	case DO_NOTHING:
		return;
		break;
	case SPAWN_DINO:
		m_map->SpawnEnemyUnit( lane , unit );
		break;
	case  SPAWN_HERO:
		m_map->SpawnEnemyUnit( lane , unit );
		break;
	case  SPAWN_SPIKE:
		m_map->SpawnEnemySpike( lane , m_decision.xPosition );
		break;
	}
}

void AIController::Update()
{
	if ( m_timer->HasElapsed() )
	{
		TakeDecision();
		Execute();
		m_timer->Reset();
	}
}
