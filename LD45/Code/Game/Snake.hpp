#pragma once

#include "Game/SnakePart.hpp"
#include <vector>

class Snake
{
public:

	std::vector<SnakePart> snake;

	Snake();

	SnakePart GetSnakeHead();

	void AddPart();

	void MoveSnakeRight(float deltaseconds);

	void MoveSnakeUp(float deltaseconds);

	void RenderSnake();

	


};