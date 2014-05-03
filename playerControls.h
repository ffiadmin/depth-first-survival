/*
Implemented by Garrett Young
Camera and movement controls
*/

#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <d3dx9math.h>
#include "d3dApp.h"
#include "d3dUtil.h"
#include "constants.h"
#include "Maze.h"

//Include the class for the player here
//Change class of player variable accordingly
#include "GameObject.h"

class playerControls
{
public:
	playerControls(void);
	~playerControls(void);

	Maze *maze;

	//Call this in update
	void movePlayer(GameObject &player, float playerSpeed, Vector3 dir, bool perspective);
	Vector3 movementFP(Vector3 dir);

	void init(Maze *m);
	//Call this in onResize
	void resize(D3DXMATRIX &mProj, float FOV, float nearClip, float farClip);
	//Call this in initApp
	void setHeightAndWidth(int wH, int wW) { windowHeight = wH; windowWidth = wW;}
	//Call this in update
	void update(float &theta, float &phi, float &radius, float headPos, float &dt, GameObject player, D3DXMATRIX &view, D3DXVECTOR3 &eyePos, bool perspective);
	Vector3 getTarget() {return target;}

private : 
	int windowHeight;
	int windowWidth;
	Vector3 target;
};

