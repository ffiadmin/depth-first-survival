#include "TrackingEnemies.h"

TrackingEnemies::TrackingEnemies()
{
	reverse = false;
	moves = 10;
}

void TrackingEnemies::setPath(Maze* maze, Location startCell, Location endCell)
{
	crap = endCell;
	if(startCell.x<0) startCell.x=0;
	if(startCell.z<0) startCell.z=0;
	if(endCell.x<0) startCell.x = 0;
	if(endCell.x<0) endCell.x = 0;
	if(startCell.x>9) startCell.x=9;
	if(startCell.z>9) startCell.x=9;
	if(endCell.x>9) endCell.x=9;
	if(endCell.z>9) endCell.z = 9;
	path = maze->solve(startCell,endCell);
	forward = &path;
	current = forward;
	path2 = maze->solve(endCell,startCell);
	backward = &path2;
	playerCell = maze->pxToCell(endCell);
	end = getEnd(current);
	s = startCell;
	e = end->previous->location;
	int stuff = 0;
}

Solution* TrackingEnemies::getEnd(Solution* path)
{
	if(path->end || path->next->location.x<=-1 || path->next->location.z<=-1)
	{
		path->end = true;
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
	/*bool swap = false;

	if(current->location.x == -1 || current->location.z == -1 || current->location.x > 1000 || current->location.z > 1000)
	{
		reverse = !reverse;
		if(reverse)
		{
			current = current->previous;
		}
		else
		{
			current = current->next;
		}
	}*/

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

	if(((getPosition().x <= next.x+2 && getPosition().x >= next.x-2) && (getPosition().z <= next.z+2 && getPosition().z >= next.z-2)))
	{
		/*if(!track && ((!reverse && (current->end||current->next->location.x==-1||current->next->location.z==-1||current->next->location.x==-1||current->next->location.z==-1||current->end)) || (reverse && (current->start))))
		{
			reverse = !reverse;
		}*/
		/*if(!reverse)
		{
			if(!current->end && current->next->location.x !=-1 && current->next->location.z!=-1 && current->location.x!=-1 && current->location.z!=-1)
			{
				current = current->next;
				next = maze->cellToPx(current->location);
			}
			else
			{
				reverse = !reverse;
			}
		}
		else
		{
			if(!current->start && current->previous->location.x!=-1 && current->previous->location.z!=-1 && current->location.x!=-1 && current->location.z!=-1)
			{
				current = current->previous;
				next = maze->cellToPx(current->location);
			}
			else
			{
				reverse = !reverse;
			}			
		}*/
		current = current->next;
		next = maze->cellToPx(current->location);
	}

	if(!track)
	{
		Location q;
		q.x = getPosition().x;
		q.z = getPosition().z;
		Location c = maze->pxToCell(q);
		if(!reverse && c.x == e.x && c.z == e.z)
		{
			current = backward;
			reverse = true;
		}
		if(reverse && c.x == s.x && c.z == s.z)
		{
			current = forward;
			reverse = false;
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