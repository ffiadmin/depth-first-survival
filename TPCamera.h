#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <d3dx9math.h>
#include "d3dApp.h"
#include "d3dUtil.h"
#include "constants.h"

//Include the class for the player here
//Change class of player variable accordingly
#include "GameObject.h"

class TPCamera {
public : 
	TPCamera();
	~TPCamera();

	//Call this in onResize
	void resize(D3DXMATRIX &mProj, float FOV, float nearClip, float farClip);
	//Call this in initApp
	void setHeightAndWidth(int wH, int wW) { windowHeight = wH; windowWidth = wW;}
	//Call this in update
	void update(float &theta, float &phi, float &radius, float headPos, float &dt, GameObject player, D3DXMATRIX &view, D3DXVECTOR3 &eyePos);
	Vector3 getTarget() {return target;}
private : 
	D3DXVECTOR3 target;
	int windowHeight;
	int windowWidth;
};

