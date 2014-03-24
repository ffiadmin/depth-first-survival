#include "TPMovement.h"

TPMovement::TPMovement(void) { }

TPMovement::~TPMovement(void) { }

void TPMovement::movePlayer(GameObject &player, float playerSpeed, float theta)
{
	player.setVelocity(movementFP(theta) * playerSpeed);
}

Vector3 TPMovement::movementFP(float mTheta)
{
	Vector3 direction = VectorZero;
	if( mTheta <= ToRadian(45.0f) && mTheta >= ToRadian(0.0f))
	{
		if(GetAsyncKeyState('S') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('D') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('W') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('A') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}
	else if( mTheta <= ToRadian(135.0f) && mTheta > ToRadian(45.0f))
	{
		if(GetAsyncKeyState('A') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('S') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('D') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('W') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}
	else if( mTheta > ToRadian(135.0f) && mTheta <= ToRadian(225.0f))
	{
		if(GetAsyncKeyState('W') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('A') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('S') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('D') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}
	else if( mTheta <= ToRadian(315.0f) && mTheta > ToRadian(225.0f))
	{
		if(GetAsyncKeyState('D') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('W') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('A') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('S') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}
	if( mTheta > ToRadian(315.0f) && mTheta <= ToRadian(360.0f))
	{
		if(GetAsyncKeyState('S') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('D') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('W') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('A') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}

	if(direction != VectorZero)
		Normalize(&direction, &direction);

	return direction;
}
