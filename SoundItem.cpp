#include "SoundItem.h"
#include <math.h>

SoundItem::SoundItem(Audio* a, Vector3 pos, double r)
{
	audio = a;
	position = pos;
	radius = r;
}

void SoundItem::setSounds(string* soundCues, int numSounds)
{
	sounds = soundCues;
	volumeLevels = numSounds;
	increment = radius/volumeLevels;
}

void SoundItem::setRadius(double r)
{
	radius = r;
	increment = radius/volumeLevels;
}

void SoundItem::playSound(Vector3 location)
{
	Vector3 distanceVector = location - position;
	double distance = sqrt((pow(distanceVector.x,2)+pow(distanceVector.y,2)+pow(distanceVector.z,2)));
	distance = abs(distance);
	int soundNum = distance / increment;
	//dont play anything if they are too far away
	if(soundNum > volumeLevels)
	{
		return;
	}
	else
	{
		audio->playCue(sounds[soundNum].c_str());
	}
}