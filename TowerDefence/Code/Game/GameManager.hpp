#pragma once

struct GameManager
{
	int playerCoins = 100;
	int enemyCoins = 100;

	float playerHealth = 100.f;
	float enemyHealth = 100.f;

	int playerCurrentHeroLevel = 0;
	int playerCurrentDinoLevel = 0;
	int enemyCurrentHeroLevel = 0;
	int enemyCurrentDinoLevel = 0;

	int playerCurrentHeroCost = 10;
	int playerCurrentDinoCost = 50;
	int playerCurrentSpikeCost = 10;
	int enemyCurrentHeroCost = 10;
	int enemyCurrentDinoCost = 50;
	int enemyCurrentSpikeCist = 10;

	int playerHeroUpgradeCost = 70;
	int playerDinoUpgradeCost = 90;
	int playerSpikeUpgradeCost = 60;

	int towerHitPoints = 20;
	int heroHitPoints = 15;
	int spikeHitPoints = 15;
	int dinoTowerHitPoints = 50;

};