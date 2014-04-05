#include "FPCamera.h"

FPCamera::FPCamera() { }

FPCamera::~FPCamera() { }

void FPCamera::resize(D3DXMATRIX &Proj, float FOV, float nearClip, float farClip) {
	float aspectRatio = (float)windowWidth/windowHeight;
	D3DXMatrixPerspectiveFovLH(&Proj, FOV, aspectRatio, nearClip, farClip);
}

void FPCamera::update(float &theta, float &phi, float &radius, float headPos, float &dt, GameObject player, D3DXMATRIX &view, D3DXVECTOR3 &eyePos)
{
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
	target.z = -radius*sinf(phi)*cosf(theta) + player.getPosition().z;
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&view, &eyePos, &target, &up);
}
