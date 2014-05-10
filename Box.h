//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H

#include "d3dUtil.h"
#include "Geometry.h"
#include "Vertex.h"

class Box : public Geometry
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, float scale);
	void draw();
};

#endif // BOX_H
