#pragma once

#include "Geometry.h"

class FlashLight : public Geometry
{
public:
	FlashLight();
	~FlashLight();

	void init(ID3D10Device* device, D3DXCOLOR c);
	void draw();
private:
	D3DXMATRIX world;
};