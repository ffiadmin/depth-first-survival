#pragma once
#include "d3dUtil.h"
#include "Geometry.h"
#include "Vertex.h"

class Monster : public Geometry
{
public:
	Monster(void);
	~Monster(void);

	void init(ID3D10Device* device, float scale);
	void draw();
};

