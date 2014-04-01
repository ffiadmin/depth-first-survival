#include "flashLightObject.h"

FlashLightObject::~FlashLightObject()
{
	delete lightSource;
}

FlashLightObject::FlashLightObject()
{
	lightSource = new Light();
	lightSource->ambient  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	lightSource->diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	lightSource->specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	lightSource->att.x    = 0.0f;
	lightSource->att.y    = 0.1f;
	lightSource->att.z    = 0.0f;
	lightSource->pos = D3DXVECTOR3(10,10,10);
	//lightSource->range    = 50.0f;
}

void FlashLightObject::update(float dt)
{
	GameObject::update(dt);
	lightSource->pos = getPosition();
}