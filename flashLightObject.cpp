#include "flashLightObject.h"

FlashLightObject::~FlashLightObject()
{}

FlashLightObject::FlashLightObject()
{
	timer = 0;
	lightSource.ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	lightSource.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	lightSource.specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	lightSource.att.x    = 1.0f;
	lightSource.att.y    = 0.0f;
	lightSource.att.z    = 0.0f;
	lightSource.spotPow  = 80;
	lightSource.range    = 100;
	lightSource.pos = D3DXVECTOR3(10,20,10);
	lightSource.dir = D3DXVECTOR3(0, 1, 0);	
	lightSource.lightType = 2;
	decreaseIncrement = 5;
}

void FlashLightObject::update(float dt)
{
	timer+=dt;
	if(timer>decreaseIncrement)
	{
		timer = 0;
		lightSource.diffuse.r-=0.2;
		lightSource.diffuse.g-=0.2;
		lightSource.diffuse.b-=0.2;
		lightSource.diffuse.r = max(lightSource.diffuse.r,0);
		lightSource.diffuse.g = max(lightSource.diffuse.g,0);
		lightSource.diffuse.b = max(lightSource.diffuse.b,0);
	}
	GameObject::update(dt);
	lightSource.pos = getPosition();
	//Normalize(&lightSource.dir,&getRotation());
}

void FlashLightObject::getBattery()
{
	timer = 0;
	lightSource.diffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
}

void FlashLightObject::init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx, ID3D10EffectMatrixVariable* fx2,float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	flashLight.init(device,1,"flashLight.txt");
	GameObject::init(&flashLight,fx,fx2,r,pos,vel,sp,s);
}

void FlashLightObject::setRotation(Vector3 r)
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