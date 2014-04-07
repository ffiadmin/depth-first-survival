#include "GameObject.h"

GameObject::GameObject()
{
	radius = 0;
	speed = 0;
	active = true;
	rotation = Vector3(0,0,0);
	Identity(&world);
	mfxDiffuseMapVar = 0;
	mfxSpecMapVar = 0;
	mDiffuseMapRV = 0;
	mSpecMapRV = 0;
}

GameObject::~GameObject()
{
	geo = NULL;
}

void GameObject::setTex(ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc, wchar_t* diffuseMap, wchar_t* specMap)
{
	mfxDiffuseMapVar = diffuseLoc;
	mfxSpecMapVar = specLoc;
	HR(D3DX10CreateShaderResourceViewFromFile(geo->md3dDevice, 
		diffuseMap, 0, 0, &mDiffuseMapRV, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(geo->md3dDevice, 
		specMap, 0, 0, &mSpecMapRV, 0 ));
}

void GameObject::draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique)
{
	if (!active)
		return;

	geo->md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//put these in the specific draws for the objects
	if(mfxDiffuseMapVar)
		mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	if(mfxSpecMapVar)
		mfxSpecMapVar->SetResource(mSpecMapRV);

	mWVP = getWorldMatrix()  *model*projection;
	//sends world view matrix to the shader
	//handle to matrix variable in the shader
	mfxWVPVar->SetMatrix((float*)&mWVP);
	//set the world matrix
	mfxWorldVar->SetMatrix((float*)&world);

	//sets the technique
	setMTech(technique);
	
    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        geo->draw();
    }
}

void GameObject::init(Geometry *g, ID3D10EffectMatrixVariable* fx, ID3D10EffectMatrixVariable* fx2,float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	mfxWVPVar = fx;
	mfxWorldVar = fx2;
	geo = g;
	radius = r;
	radius *= 1.01; //fudge factor
	position = pos;
	velocity = vel;
	speed = sp;
	scale = s;
	radiusSquared = radius * radius;
}

void GameObject::update(float dt)
{
	Matrix rotXM, rotYM, rotZM, transM, scaleM;
	position += velocity * dt;
	Identity(&world);
	RotateX(&rotXM,rotation.x);
	RotateY(&rotYM,rotation.y);
	RotateZ(&rotZM,rotation.z);
	Scale(&scaleM,scale.x,scale.y,scale.z);
	Translate(&transM, position.x, position.y, position.z);

	world =  scaleM * rotXM * rotYM * rotZM * transM;

}

bool GameObject::collided(GameObject *gameObject)
{
	//extra code that makes sure something is active might change for specific circumstances
	if(!gameObject->getActiveState())
	{
		return false;
	}
	Vector3 diff = position - gameObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + gameObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}