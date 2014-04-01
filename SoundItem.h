#pragma once

#include <string>
#include "audio.h"
#include "constants.h"
#include "d3dUtil.h"
using std::string;

class SoundItem
{
public:
	SoundItem(Audio* a, Vector3 pos, double r);
	void setSounds(string* soundCues, int numSounds);
	void setRadius(double r);
	void playSound(Vector3 location);
private:
	int volumeLevels;
	string* sounds;
	Audio* audio;
	Vector3 position;
	double radius;
	int increment;
};