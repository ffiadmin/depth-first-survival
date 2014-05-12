#pragma once

#include "GameObject.h"
#include "constants.h"
#include "Maze.h"

class TrackingEnemies : public GameObject
{
public:
	TrackingEnemies();
	void setPath(Maze* maze, Location startCell, Location endCell);
	void update(float dt, GameObject* player, bool track, Maze* maze);
	Solution* getEnd(Solution* path);
private:
	Solution path;
	bool reverse;
	Solution* current;
	Solution* end;
	Location playerCell;
};