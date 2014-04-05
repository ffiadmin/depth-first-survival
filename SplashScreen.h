#pragma once

#include "GameObject.h"
#include "constants.h"

class SplashScreen
{
	void init(GameObject* display,Vector3 location);
	void setImage(wchar_t* imageName);
};