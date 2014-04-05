
#ifndef QUAD_H
#define QUAD_H

#include "d3dUtil.h"
#include "Geometry.h"

class Quad : public Geometry
{
public:
	Quad();
	~Quad();

	void init(ID3D10Device* device, float scale, D3DXCOLOR c);
	void draw();
};


#endif 
