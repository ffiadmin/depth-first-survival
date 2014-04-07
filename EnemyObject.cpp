#include "EnemyObject.h"

void EnemyObject::init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	enemy.init(device,1,"enemy.txt");
	GameObject::init(&enemy,fx,fx2,r,pos,vel,sp,s);
}

void EnemyObject::update(float dt, GameObject* player)
{
	Vector3 direction = player->getPosition()-getPosition();
	Normalize(&direction,&direction);
	setVelocity(direction *15);
	GameObject::update(dt);
}