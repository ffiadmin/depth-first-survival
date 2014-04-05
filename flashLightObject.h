#pragma once

#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"
#include "constants.h"
#include <math.h>

class FlashLightObject : public GameObject
{
public:
	FlashLightObject();
	~FlashLightObject();
	void init(ID3D10Device* device,ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	void update(float dt);
	void setRotation(Vector3 r);
	void getBattery();
	Light getLight() {return lightSource;}
	Light lightSource;
private:
	float timer;
	int decreaseIncrement;
	int powerLevel;
	float powerLossValue;
	Mesh flashLight;
};