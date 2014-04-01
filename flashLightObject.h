#pragma once

#include "GameObject.h"
#include "Light.h"

class FlashLightObject : public GameObject
{
public:
	FlashLightObject();
	~FlashLightObject();
	void update(float dt);
	Light* getLight() {return lightSource;}
private:
	Light* lightSource;
};