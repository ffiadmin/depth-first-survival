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

void playerControls::init(Maze *m, Input *i)
{
	maze = m;
	input = i;

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	center.x = width / 2;
	center.y = height / 2;
	prev = center;

	ShowCursor(FALSE);
}

void playerControls::movePlayer(GameObject &player, float playerSpeed, Vector3 dir, bool perspective)
{
	player.setVelocity(playerSpeed * movementFP(dir,perspective));
}

void playerControls::moveObject(GameObject &player, float playerSpeed, Vector3 dir, bool perspective)
{
	player.setVelocity(playerSpeed * dir);
}

Vector3 playerControls::movementFP(Vector3 dir, bool perspective)
{
	Vector3 direction = Vector3(0,0,0);
	if(perspective)
	{
		if(GetAsyncKeyState('W') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('A') & 0x8000)
		{
			direction.z = -1;
		}
		if(GetAsyncKeyState('S') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('D') & 0x8000)
		{
			direction.z = 1;
		}

		if(direction != VectorZero)
			Normalize(&direction, &direction);

		return direction;
	}

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

void playerControls::update(float &theta, float &phi, float &radius, float headPos, float &dt, GameObject player, D3DXMATRIX &view, D3DXVECTOR3 &eyePos, bool perspective, bool power, float sensitivity)
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
		float mPhi = 0.1f, mTheta = ToRadian(90.0f);
		float mRadius = maze->getDimension().x + 20.0f;
		// Restrict the angle mPhi and radius mRadius.
		//if( phi < 0.1f )	phi = 0.1f;
		//if( phi > PI-0.1f)	phi = PI-0.1f;

		//Loop mTheta back to zero for maths sake
		//if( theta >= (PI * 2) ) theta = ToRadian(0.1f);
		//if( theta <= 0.0f ) theta = ToRadian(359.99f);

		//if( radius < 0.0f) radius = 0.0f;

		// Convert Spherical to Cartesian coordinates: mPhi measured from +y
		// and mTheta measured counterclockwise from -z.
		if(!power)
		{
			if(GetAsyncKeyState(VK_RIGHT) & 0x8000)	mTheta += sensitivity*dt;
			if(GetAsyncKeyState(VK_LEFT) & 0x8000)	mTheta -= sensitivity*dt;
			if(GetAsyncKeyState(VK_UP) & 0x8000)	mPhi -= sensitivity*dt;
			if(GetAsyncKeyState(VK_DOWN) & 0x8000)	mPhi += sensitivity*dt;
			//if(GetAsyncKeyState('O') & 0x8000)	radius -= 25.0f*dt;
			//if(GetAsyncKeyState('P') & 0x8000)	radius += 25.0f*dt;
			eyePos.x =  mRadius*sinf(mPhi)*sinf(mTheta) + maze->getDimension().x/2.0f;
			eyePos.z = -mRadius*sinf(mPhi)*cosf(mTheta) + maze->getDimension().z/2.0f;
			eyePos.y =  mRadius*cosf(mPhi);

			// Build the view matrix.
			target = Vector3(0.0f, 0.0f, 0.0f);
			target.x = maze->getDimension().x/2.0f;
			target.y = 0.0f;
			target.z = maze->getDimension().z/2.0f;
		}
		else
		{
			mRadius = 100;
			eyePos.x =  mRadius*sinf(mPhi)*sinf(mTheta) + player.getPosition().x;
			eyePos.z = -mRadius*sinf(mPhi)*cosf(mTheta) + player.getPosition().z;
			eyePos.y =  mRadius*cosf(mPhi);

			// Build the view matrix.
			target = Vector3(0.0f, 0.0f, 0.0f);
			target.x = player.getPosition().x;
			target.y = 0.0f;
			target.z = player.getPosition().z;
		}
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&view, &eyePos, &target, &up);
		Normalize(&target,&target);
	}
	else
	{
		maze->setCeilingVisibility(true);

		cur.x = input->getMouseX();
		cur.y = input->getMouseY();

		/*if (cur - prev != ZERO_2VEC) {
			D3DXVECTOR2 dir(0, 0);

			dir.x = prev.x - cur.x;
			dir.y = prev.y - cur.y;

		//Move left
			if (prev.x > cur.x) theta += sensitivity * dt;

		//Move right
			if (prev.x < cur.x) theta -= sensitivity * dt;
			
		//Move up
			if (prev.y < cur.y) phi += sensitivity * dt;

		//Move down
			if (prev.y > cur.y) phi -= sensitivity * dt;

		//Normalize the direction
			D3DXVec2Normalize(&dir, &dir);

			theta += dir.x;
			phi += dir.y;

		//Reset mouse position for bounded mouse movement
			SetCursorPos(center.x, center.y);
		}*/

		if(GetAsyncKeyState(VK_RIGHT) & 0x8000)	theta -= sensitivity*dt;
		if(GetAsyncKeyState(VK_LEFT) & 0x8000)	theta += sensitivity*dt;
		if(GetAsyncKeyState(VK_UP) & 0x8000)	phi -= sensitivity*dt;
		if(GetAsyncKeyState(VK_DOWN) & 0x8000)	phi += sensitivity*dt;
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
