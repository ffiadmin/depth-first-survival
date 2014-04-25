#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Light.h"

class LightObject : public GameObject
{
public:
	LightObject();
	void init(ID3D10Device* device,ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	Light* getLight() {return &lightSource;}
	void setColor(D3DXCOLOR c);
	void update(float dt);
	void setRotation(Vector3 r);
private:
	Light lightSource;
	Mesh lightGeo;
};