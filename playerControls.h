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
#include "input.h"

//Include the class for the player here
//Change class of player variable accordingly
#include "GameObject.h"
#define ZERO_2VEC D3DXVECTOR2(0, 0)

class playerControls
{
public:
	playerControls(void);
	~playerControls(void);

	Maze *maze;

	//Call this in update
	void movePlayer(GameObject &player, float playerSpeed, Vector3 dir, bool perspective);
	void moveObject(GameObject &player, float playerSpeed, Vector3 dir, bool perspective);
	Vector3 movementFP(Vector3 dir, bool perspective);

	void init(Maze *m, Input *i);
	//Call this in onResize
	void resize(D3DXMATRIX &mProj, float FOV, float nearClip, float farClip);
	//Call this in initApp
	void setHeightAndWidth(int wH, int wW) { windowHeight = wH; windowWidth = wW;}
	//Call this in update
	void update(float &theta, float &phi, float &radius, float headPos, float &dt, GameObject player, D3DXMATRIX &view, D3DXVECTOR3 &eyePos, bool perspective, bool power, float sensitivity);
	Vector3 getTarget() {return target;}

private : 
	D3DXVECTOR2 center;
	D3DXVECTOR2 cur;
	D3DXVECTOR2 prev;
	Input *input;
	int windowHeight;
	int windowWidth;
	Vector3 target;
};

