#pragma once

#include "d3dUtil.h"

class Geometry
{
public:
	Geometry() {mNumVertices = 0; mNumFaces = 0; md3dDevice = 0; mVB = 0; mIB = 0;}
	virtual void draw() = 0;
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	
};