#pragma once

#include "Geometry.h"
#include "constants.h"
#include "Vertex.h"

class Billboard : public Geometry
{
public:
	Billboard();
	~Billboard();
	void init(ID3D10Device* device, Vector3 position, Vector2 size);
	void draw();
private:

};