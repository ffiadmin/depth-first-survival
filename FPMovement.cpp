#include "FPMovement.h"

FPMovement::FPMovement(void) { }

FPMovement::~FPMovement(void) { }

void FPMovement::movePlayer(GameObject &player, float playerSpeed, Vector3 dir)
{
	player.setVelocity(movementFP(dir) * playerSpeed);
}

Vector3 FPMovement::movementFP(Vector3 dir)
{
	Vector3 direction = Vector3(0,0,0);
	//D3DXVec3Transform(&start,&lightSource.dir,&rotateX)
	if(GetAsyncKeyState('W') & 0x8000)
		direction = Vector3(dir.x,0,dir.z);
	if(GetAsyncKeyState('A') & 0x8000)
	{
		Matrix r;
		RotateY(&r,ToRadian(-90.0f));
		Vector4 d;
		D3DXVec3Transform(&d,&dir,&r);
		direction = Vector3(d.x,0,d.z);
	}
	if(GetAsyncKeyState('S') & 0x8000)
		direction = Vector3(dir.x,0,dir.z)*-1;
	if(GetAsyncKeyState('D') & 0x8000)
	{
		Matrix r;
		RotateY(&r,ToRadian(90.0f));
		Vector4 d;
		D3DXVec3Transform(&d,&dir,&r);
		direction = Vector3(d.x,0,d.z);
	}
	
	if(direction != VectorZero)
		Normalize(&direction, &direction);

	return direction;
}
