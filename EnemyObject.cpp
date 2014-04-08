#include "EnemyObject.h"

void EnemyObject::init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	setHealth(5);
	string types[3];
	types[0] = "enemy.txt";
	types[1] = "garretEnemy.txt";
	types[2] = "donut.txt";
	int num = rand()%3;
	enemy.init(device,1,types[num]);
	GameObject::init(&enemy,fx,fx2,r,pos,vel,sp,s);
	ghostLight.ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	ghostLight.diffuse  = D3DXCOLOR(0.2f, 0.1f, 0.0f, 1.0f);
	ghostLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	ghostLight.att.x    = 1.0f;
	ghostLight.att.y    = 0.0f;
	ghostLight.att.z    = 0.0f;
	ghostLight.spotPow  = 10;
	ghostLight.range    = 100;
	ghostLight.pos = D3DXVECTOR3(10,20,10);
	ghostLight.dir = D3DXVECTOR3(0, -1, 0);	
	ghostLight.lightType.x = 2;
}

void EnemyObject::update(float dt, GameObject* player)
{
	if(!getActiveState())
		return;
	ghostLight.pos = getPosition() - (ghostLight.dir*5);
	Vector3 direction = player->getPosition()-getPosition();
	Normalize(&direction,&direction);
	setVelocity(direction *15);
	GameObject::update(dt);
	if(getHealth()<=0)
		setInActive();
}