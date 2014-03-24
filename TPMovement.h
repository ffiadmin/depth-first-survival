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

class TPMovement
{
public:
	TPMovement(void);
	~TPMovement(void);

	//Call this in update
	void movePlayer(GameObject &player, float playerSpeed, float theta);
	Vector3 movementFP(float theta);
};

