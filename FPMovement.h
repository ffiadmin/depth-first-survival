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

class FPMovement
{
public:
	FPMovement(void);
	~FPMovement(void);

	//Call this in update
	void movePlayer(GameObject &player, float playerSpeed, Vector3 dir);
	Vector3 movementFP(Vector3 dir);
};

