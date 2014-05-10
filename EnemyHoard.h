#pragma once
#include "EnemyObject.h"

class EnemyHoard
{
public:
	EnemyHoard(int numE, float timeBetweenGen,int gridX, int gridZ);
	~EnemyHoard();
	void update(float dt,GameObject* player, Vector3 playerDir, bool topDown);
	void draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique);
	int getNumEnemies() {return numEnemies;}
	void setTex(ID3D10ShaderResourceView* diffuseTexture, ID3D10ShaderResourceView* specTexture);
	void setTexLocVariable(ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc);
	float getTBG() {return timeBetweenGeneration;}
	void setTBG(float t) {timeBetweenGeneration = t;}
	EnemyObject* getEnemies() {return enemies;}
	void init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	void setBlend(ID3D10BlendState* b) {mTransparentBS = b;}
private:
	EnemyObject* enemies;
	int numEnemies;
	float timer;
	float timeBetweenGeneration;
	ID3D10BlendState* mTransparentBS;
	int maxX;
	int maxZ;
};