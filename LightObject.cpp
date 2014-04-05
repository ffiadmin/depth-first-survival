#include "LightObject.h"

LightObject::LightObject()
{
	lightSource.ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	lightSource.diffuse  = D3DXCOLOR(0.2f, 0.1f, 0.0f, 1.0f);
	lightSource.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	lightSource.att.x    = 1.0f;
	lightSource.att.y    = 0.0f;
	lightSource.att.z    = 0.0f;
	lightSource.spotPow  = 10;
	lightSource.range    = 100;
	lightSource.pos = D3DXVECTOR3(10,20,10);
	lightSource.dir = D3DXVECTOR3(0, -1, 0);	
	lightSource.lightType.x = 2;
}

void LightObject::setColor(D3DXCOLOR c)
{
	lightSource.diffuse = c;
}

void LightObject::init(ID3D10Device* device,ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	lightGeo.init(device,1,"light.txt");
	GameObject::init(&lightGeo,fx,fx2,r,pos,vel,sp,s);
}

void LightObject::update(float dt)
{
	GameObject::update(dt);
	lightSource.pos = getPosition() - (lightSource.dir*5);

}

void LightObject::setRotation(Vector3 r)
{
	GameObject::setRotation(r);
	Matrix rotateX, rotateY, rotateZ;
	RotateX(&rotateX,r.x);
	RotateY(&rotateY,r.y);
	RotateZ(&rotateZ,r.z);
	Vector4 start;
	//rotate x
	D3DXVec3Transform(&start,&lightSource.dir,&rotateX);
	lightSource.dir = Vector3(start.x,start.y,start.z);
	//rotate y
	D3DXVec3Transform(&start,&lightSource.dir,&rotateY);
	lightSource.dir = Vector3(start.x,start.y,start.z);
	//rotate z
	D3DXVec3Transform(&start,&lightSource.dir,&rotateZ);
	lightSource.dir = Vector3(start.x,start.y,start.z);

	Normalize(&lightSource.dir,&lightSource.dir);
}