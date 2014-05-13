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
	Location crap;
	Solution path;
	Solution path2;
	bool reverse;
	Solution* current;
	Solution* forward;
	Solution* backward;
	Solution* end;
	int moves;
	Location s;
	Location e;
	Location playerCell;
};