#pragma once

#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"
#include "constants.h"
#include "Box.h"
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
	void turnOn() {on = true;}
	int getPowerLevel() { return powerLevel;}
	bool getOn() {return on;}
	Light getLight() {return lightSource;}
	Light lightSource;
	bool hitTarget(GameObject* g);
	GameObject hitBox;
private:
	
	Box b;
	float timer;
	bool on;
	int decreaseIncrement;
	int powerLevel;
	float powerLossValue;
	float hitDistance;
	float hitDimension;
	Mesh flashLight;
	bool debounce;
};