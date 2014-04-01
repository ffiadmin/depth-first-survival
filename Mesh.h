#pragma once

#include "Geometry.h"
#include "Vertex.h"
#include "constants.h"
#include <vector>
using std::vector;

class Mesh : public Geometry
{
public:
	Mesh();
	void init(ID3D10Device* device, float scale);
	void draw();
private:
	void makePolygon(Vector3 base[], int numPoints);
	void surfRev(int numSlices, Vector3 linePoints[], int numPoints);
	vector<Vertex> vertices;
	vector<DWORD> indices;
};