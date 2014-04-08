#pragma once
#include "EnemyObject.h"

class EnemyHoard
{
public:
	EnemyHoard(int numE, float timeBetweenGen,int gridX, int gridZ);
	~EnemyHoard();
	void update(float dt,GameObject* player);
	void draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique);
	int getNumEnemies() {return numEnemies;}
	void setTex(ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc, wchar_t* diffuseMap, wchar_t* specMap);
	float getTBG() {return timeBetweenGeneration;}
	void setTBG(float t) {timeBetweenGeneration = t;}
	EnemyObject* getEnemies() {return enemies;}
	void init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
private:
	EnemyObject* enemies;
	int numEnemies;
	float timer;
	float timeBetweenGeneration;
	int maxX;
	int maxZ;
};