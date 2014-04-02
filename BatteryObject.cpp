#include "BatteryObject.h"

void BatteryObject::init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx, ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc, wchar_t* diffuseMap, wchar_t* specMap, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	battery.init(device,1,"battery.txt");
	GameObject::init(&battery,fx,diffuseLoc,specLoc,diffuseMap,specMap,r,pos,vel,sp,s);
}
