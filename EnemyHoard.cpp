#include "EnemyHoard.h"

EnemyHoard::~EnemyHoard()
{
	//delete enemies;
}

EnemyHoard::EnemyHoard(int numE, float timeBetweenGen,int gridX, int gridZ)
{
	numEnemies = numE;
	timeBetweenGeneration = timeBetweenGen;
	maxX = gridX;
	maxZ = gridZ;
	enemies = new EnemyObject[numE];
	timer = 0;
}

void EnemyHoard::init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	for(int i = 0; i < numEnemies; i++)
	{
		enemies[i].init(device,fx,fx2,r,pos,vel,sp,s);
		enemies[i].setInActive();
	}
}

void EnemyHoard::draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique)
{
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	for(int i = 0; i < numEnemies; i++)
	{
		if(!enemies[i].getHit())
			enemies[i].getGeo()->md3dDevice->OMSetBlendState(mTransparentBS, blendFactors, 0xffffffff);
		else
			enemies[i].getGeo()->md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
		enemies[i].draw(model,projection,technique);
	}
}

void EnemyHoard::update(float dt,GameObject* player, Vector3 playerDir, bool topDown)
{
	timer+=dt;
	if(timer>=timeBetweenGeneration)
	{
		int x = rand()%maxX;
		int z = rand()%maxZ;
		for(int i = 0; i < numEnemies; i++)
		{
			if(!enemies[i].getActiveState())
			{
				enemies[i].setActive();
				//set enemies position
				enemies[i].setPosition(Vector3(x,6,z));
				//reset their health
				enemies[i].setHealth(10);
				break;
			}
		}
		timer = 0;
	}
	for(int i = 0; i < numEnemies; i++)
	{
		enemies[i].update(dt,player,playerDir,topDown);
	}
}

void EnemyHoard::setTex(ID3D10ShaderResourceView* diffuseTexture, ID3D10ShaderResourceView* specTexture)
{
	for(int i = 0; i < numEnemies; i++)
	{
		enemies[i].setTex(diffuseTexture,specTexture);
	}
}

void EnemyHoard::setTexLocVariable(ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc)
{
	for(int i = 0; i < numEnemies; i++)
	{
		enemies[i].setTexLocVariable(diffuseLoc,specLoc); 
	}
}