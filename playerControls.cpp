/*
Implemented by Garrett Young
Camera and movement controls
*/

#include "playerControls.h"


playerControls::playerControls(void)
{
}


playerControls::~playerControls(void)
{
}

void playerControls::init(Maze *m)
{
	maze = m;
}

void playerControls::movePlayer(GameObject &player, float playerSpeed, Vector3 dir, bool perspective)
{
	if(perspective)
	{
		
	}
	else
	{
		player.setVelocity(movementFP(dir) * playerSpeed);
	}
}

Vector3 playerControls::movementFP(Vector3 dir)
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

void playerControls::resize(D3DXMATRIX &Proj, float FOV, float nearClip, float farClip) {
	float aspectRatio = (float)windowWidth/windowHeight;
	D3DXMatrixPerspectiveFovLH(&Proj, FOV, aspectRatio, nearClip, farClip);
}

void playerControls::update(float &theta, float &phi, float &radius, float headPos, float &dt, GameObject player, D3DXMATRIX &view, D3DXVECTOR3 &eyePos, bool perspective)
{
	if(perspective)
	{
		if(radius == 0.0f)
		{
			maze->setCeilingVisibility(true);
		}
		else
		{
			maze->setCeilingVisibility(false);
		}
		float mPhi = 0.1f, mTheta = 0.0f;
		float mRadius = maze->getDimension().x + 20.0f;
		/*if(GetAsyncKeyState('J') & 0x8000)	theta -= 2.0f*dt;
		if(GetAsyncKeyState('L') & 0x8000)	theta += 2.0f*dt;
		if(GetAsyncKeyState('I') & 0x8000)	phi -= 2.0f*dt;
		if(GetAsyncKeyState('K') & 0x8000)	phi += 2.0f*dt;
		if(GetAsyncKeyState('O') & 0x8000)	radius -= 25.0f*dt;
		if(GetAsyncKeyState('P') & 0x8000)	radius += 25.0f*dt;*/

		// Restrict the angle mPhi and radius mRadius.
		//if( phi < 0.1f )	phi = 0.1f;
		//if( phi > PI-0.1f)	phi = PI-0.1f;

		//Loop mTheta back to zero for maths sake
		//if( theta >= (PI * 2) ) theta = ToRadian(0.1f);
		//if( theta <= 0.0f ) theta = ToRadian(359.99f);

		//if( radius < 0.0f) radius = 0.0f;

		// Convert Spherical to Cartesian coordinates: mPhi measured from +y
		// and mTheta measured counterclockwise from -z.
		eyePos.x =  mRadius*sinf(mPhi)*sinf(mTheta) + maze->getDimension().x/2.0f;
		eyePos.z = -mRadius*sinf(mPhi)*cosf(mTheta) + maze->getDimension().z/2.0f;
		eyePos.y =  mRadius*cosf(mPhi);

		// Build the view matrix.
		target = Vector3(0.0f, 0.0f, 0.0f);
		target.x = maze->getDimension().x/2.0f;
		target.y = 0.0f;
		target.z = maze->getDimension().z/2.0f;
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&view, &eyePos, &target, &up);
		Normalize(&target,&target);
	}
	else
	{
		maze->setCeilingVisibility(true);
		if(GetAsyncKeyState('L') & 0x8000)	theta -= 2.0f*dt;
		if(GetAsyncKeyState('J') & 0x8000)	theta += 2.0f*dt;
		if(GetAsyncKeyState('I') & 0x8000)	phi -= 2.0f*dt;
		if(GetAsyncKeyState('K') & 0x8000)	phi += 2.0f*dt;
		//Don't need for First Person
		//if(GetAsyncKeyState('O') & 0x8000)	radius -= 25.0f*dt;
		//if(GetAsyncKeyState('P') & 0x8000)	radius += 25.0f*dt;

		// Restrict the angle mPhi and radius mRadius.
		if( phi < 0.1f )	phi = 0.1f;
		if( phi > PI-0.1f)	phi = PI-0.1f;

		//Loop mTheta back to zero for maths sake
		if( theta >= (PI * 2) ) theta = ToRadian(0.1f);
		if( theta <= 0.0f ) theta = ToRadian(359.99f);

		if( radius < 0.0f) radius = 0.0f;

		// Convert Spherical to Cartesian coordinates: mPhi measured from +y
		// and mTheta measured counterclockwise from -z.
		eyePos.x =  player.getPosition().x;
		eyePos.z =  player.getPosition().z;
		eyePos.y =  player.getPosition().y + headPos;

		// Build the view matrix.
		target = Vector3(0.0f, 0.0f, 0.0f);
		target.x = radius*sinf(phi)*sinf(theta) + player.getPosition().x;
		target.y = radius*cosf(phi) + player.getPosition().y;
		target.z = radius*-sinf(phi)*cosf(theta) + player.getPosition().z;
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&view, &eyePos, &target, &up);
		Normalize(&target,&target);
	}
}
