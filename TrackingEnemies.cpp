#include "TrackingEnemies.h"

TrackingEnemies::TrackingEnemies()
{
	reverse = false;
	moves = 10;
}

void TrackingEnemies::setPath(Maze* maze, Location startCell, Location endCell)
{
	path = maze->solve(startCell,endCell);
	current = &path;
	playerCell = maze->pxToCell(endCell);
	end = getEnd(current);
	int stuff = 0;
}

Solution* TrackingEnemies::getEnd(Solution* path)
{
	if(path->end || path->next->location.x==-1 || path->next->location.z==-1)
	{
		return path;
	}
	else
	{
		return getEnd(path->next);
	}
}

void TrackingEnemies::update(float dt, GameObject* player, bool track, Maze* maze)
{
	Location next = maze->cellToPx(current->location);

	//constantly add the players location to your destination
	if(track)
	{
		Location pL;
		pL.x = player->getPosition().x;
		pL.z = player->getPosition().z;
		Location playerLoc = maze->pxToCell(pL);
		if(playerLoc.x != playerCell.x || playerLoc.z != playerCell.z)
		{
			playerCell = playerLoc;
			end->end = false;
			end->next = new Solution();
			end->next->previous = end;
			end = end->next;
			end->end = true;
			end->location = playerCell;
			moves++;
			if(moves>=20)
			{
				moves = 0;
				Location enemy;
				enemy.x = getPosition().x;
				enemy.z = getPosition().z;
				enemy = maze->pxToCell(enemy);
				setPath(maze,enemy,playerCell);
			}
		}
	}

	if((getPosition().x <= next.x+2 && getPosition().x >= next.x-2) && (getPosition().z <= next.z+2 && getPosition().z >= next.z-2))
	{
		if(!track && ((!reverse && (current->next->location.x==-1||current->next->location.z==-1||current->end)) || (reverse && current->previous->start)))
		{
			reverse = !reverse;
		}
		if(!reverse)
		{
			if(current->next)
				current = current->next;
			next = maze->cellToPx(current->location);
		}
		else
		{
			if(current->previous)
				current = current->previous;
			next = maze->cellToPx(current->location);
		}
	}

	Vector3 playerDiff = player->getPosition() - getPosition();
	double distance = sqrtf(abs(Dot(&playerDiff,&playerDiff)));
	Vector3 diff = Vector3(next.x,0,next.z)-getPosition();
	
	Vector3 direction;
	Location p;
	p.x = getPosition().x;
	p.z = getPosition().z;

	if(distance>=20 || maze->collided(p))
	{
		direction = diff;
	}
	else
	{
		direction = playerDiff;
	}
	
	Normalize(&direction,&direction);
	setVelocity(direction *getSpeed());
	GameObject::update(dt);
}