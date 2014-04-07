
#ifndef GameObject_H
#define GameObject_H

//#include "d3dUtil.h"
#include "d3dUtil.h"
#include "Geometry.h"
#include "constants.h"

class GameObject
{
public:

	GameObject();
	~GameObject();

	void init(Geometry *g, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2,float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	void setTex(ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc, wchar_t* diffuseMap, wchar_t* specMap);
	void draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique);
	void update(float dt);

	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	void setRadius(float r) {radius = r; radiusSquared = (scale.x*r)*(scale.x*r);}
	float getRadiusSquare() {return radiusSquared;}
	float getRadius() {return radius;}
	Matrix getWorldMatrix() {return world;}
	void setScale(Vector3 s) {scale = s; radiusSquared = (s.x*radius)*(s.x*radius);}
	Vector3 getScale() {return scale;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	bool collided(GameObject *gameObject);

	void setRotation(Vector3 r) {rotation = r;}
	Vector3 getRotation() {return rotation;}

	int getHealth() {return health;}
	void setHealth(int h) { health = h; }

private:
	int health;
	Geometry *geo; 
	Vector3 position;
	Vector3 velocity;
	Vector3 rotation;
	//Vector3 direction;
	float speed;
	float radius;
	float radiusSquared;
	
	bool active;
	Matrix world;
	Vector3 scale;
	ID3D10EffectTechnique* mTech;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	D3DXMATRIX mWVP;

	//variables to let geometry change their texture and such
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;
};


#endif
