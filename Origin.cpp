#include "Origin.h"

Origin::Origin()
{
	line1 = 0;
	line2 = 0;
	line3 = 0;
	xLine = 0;
	yLine = 0;
	zLine = 0;
}

Origin::~Origin()
{
	delete xLine;
	delete yLine;
	delete zLine;
}

void Origin::init(Line* l1, Line* l2, Line* l3, ID3D10EffectMatrixVariable* fx, float length)
{
	line1 = l1;
	line2 = l2;
	line3 = l3;
	xLine = new LineObject();
	yLine = new LineObject();
	zLine = new LineObject();
	
	xLine->init(line1,fx,Vector3(0,0,0),length);
	yLine->init(line2,fx,Vector3(0,0,0),length);
	zLine->init(line3,fx,Vector3(0,0,0),length);

	yLine->setRotationZ(ToRadian(90));
	zLine->setRotationY(ToRadian(90));
}

void Origin::draw(D3DXMATRIX model, D3DXMATRIX projection,ID3D10EffectTechnique* technique)
{
	//draw the lines
	xLine->draw(model,projection,technique);
	yLine->draw(model,projection,technique);
	zLine->draw(model,projection,technique);
}