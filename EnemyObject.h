#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Light.h"
#include <string>
using std::string;

class EnemyObject : public GameObject
{
public:
	void init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	void update(float dt, GameObject* player);
	void decreaseHealth() {setHealth(getHealth()-1);}
	Light getLight() { return ghostLight; }
private:
	Mesh enemy;
	Light ghostLight;
};