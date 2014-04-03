#pragma once

#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"

class FlashLightObject : public GameObject
{
public:
	FlashLightObject();
	~FlashLightObject();
	void init(ID3D10Device* device,ID3D10EffectMatrixVariable* fx, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	void update(float dt);
	float getPower() {return power;}
	void setPower(float p) {power = p;}
	Light* getLight() {return lightSource;}
private:
	Light* lightSource;
	float power;
	Mesh flashLight;
};