#pragma once

#include "Geometry.h"
#include "Vertex.h"
#include "constants.h"
#include <vector>
#include <string>
#include <fstream>
using std::ifstream;
using std::string;
using std::vector;

class Mesh : public Geometry
{
public:
	Mesh();
	void init(ID3D10Device* device, float scale, string meshFile);
	void draw();
private:
	void makePolygon(Vector3 base[], int numPoints, int sectionNumber);
	void surfRev(int numSlices, Vector3 linePoints[], int numPoints);
	int numSlices;
	vector<Vertex> vertices;
	vector<DWORD> indices;
};