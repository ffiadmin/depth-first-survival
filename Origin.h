#pragma once

#include "LineObject.h"

class Origin
{
public:
	Origin();
	~Origin();
	void init(Line* l1,Line* l2,  Line* l3, ID3D10EffectMatrixVariable* fx, float length);
	void draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique);
private:
	Line* line1, *line2, *line3;
	LineObject* xLine, *yLine, *zLine;
};