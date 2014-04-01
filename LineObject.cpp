#include "LineObject.h"

LineObject::LineObject()
{
	speed = 0;
	active = true;
	Identity(&world);
	rotX = 0;
	rotY = 0;
	rotZ = 0;
}

LineObject::~LineObject()
{
	line = NULL;
}

void LineObject::draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique)
{
	if (!active)
		return;

	Matrix rotXM, rotYM, rotZM, transM, scaleM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Scale(&scaleM, scale,scale,scale);
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * scaleM * transM;

	mWVP = getWorldMatrix()*model*projection;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	setMTech(technique);

    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        line->draw();
    }

}

void LineObject::init(Line *l, ID3D10EffectMatrixVariable* fx, Vector3 pos, float s)
{
	mfxWVPVar = fx;
	line = l;
	position = pos;
	scale = s;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
}

void LineObject::update(float dt)
{
	
}

