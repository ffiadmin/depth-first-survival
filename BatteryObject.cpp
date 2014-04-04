#include "BatteryObject.h"

void BatteryObject::init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	battery.init(device,1,"battery.txt");
	GameObject::init(&battery,fx,fx2,r,pos,vel,sp,s);
}
