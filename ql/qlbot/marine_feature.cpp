#include "qlbot/marine_feature.h"


MarineFeature::MarineFeature(int paFeatureCount) : hp(0), marineCount(0), distanceFromClosestEnemy(0), featureCount(paFeatureCount), lastAction(-1), hpValue(0), weaponCD(0)
{
	quadrantSafety = new vector<int>(4,0);
}

MarineFeature::MarineFeature() : hp(0), distanceFromClosestEnemy(0), featureCount(7), lastAction(-1), hpValue(0), weaponCD(0)
{
	quadrantSafety = new vector<int>(4,0);
}

MarineFeature::~MarineFeature() 
{
}

void MarineFeature::set_hp(float hpPer, float health)
{
	hpValue = health;
    //skuska prikladat mensiemu hp velku dolezitost
    if (hpPer < 0.1)
    {
        hp = 1;
    }
    else if (hpPer < 0.2)
    {
        hp = 2;
    }
	else if (hpPer < 0.35)
	{
		hp = 3;
	}
	else if (hpPer < 0.6)
	{
		hp = 4;
	}
	else if (hpPer < 0.9)
	{
		hp = 5;
	}
	else
	{
		hp = 6;
	}
}

int MarineFeature::get_hp()
{
    return hp;
}


void MarineFeature::set_distanceFromClosestEnemy(float distance)
{
	if (distance < 1)
	{
		distanceFromClosestEnemy = 1;
	}
	else if (distance < 2)
	{
		distanceFromClosestEnemy = 2;
	}
	else if (distance < 3.5)
	{
		distanceFromClosestEnemy = 3;
	}
	else if (distance <= 4.5)
	{
		distanceFromClosestEnemy = 4;
	}
	else if (distance < 5)
	{
		distanceFromClosestEnemy = 5;
	}
	else
	{
		distanceFromClosestEnemy = 6;
	}
}



int MarineFeature::get_distanceFromClosestEnemy()
{
	return distanceFromClosestEnemy;
}




void MarineFeature::set_quadrantSafety(vector<float> *safety)
{
	//napad: najst min a max hodnotu a vsetky hodnoty vcucnut medzi ne a rozdelit na 4 casti (normalizacia sa to vola?).
	float min = FLT_MAX;
	float max = FLT_MIN;	
	for (int i = 0; i < 4; i++)
	{
		float sft = (*safety)[i];
		if (sft >= max)
			max = sft;
		if (sft <= min)
			min = sft;
	}
	for (int i = 0; i < 4; i++)
	{
		float sft = ((*safety)[i]-min)/max;
		if (sft <= 0.125)
			(*quadrantSafety)[i] = 0;
		else if (sft <= 0.375)
			(*quadrantSafety)[i] = 1;
		else if (sft <= 0.625)
			(*quadrantSafety)[i] = 2;
		else if (sft <= 0.875)
			(*quadrantSafety)[i] = 3;
		else
			(*quadrantSafety)[i] = 4;
	}
}

vector<int>* MarineFeature::get_quadrantSafety()
{
	return quadrantSafety;
}


vector<int>* MarineFeature::to_array()
{
    vector<int>* pole = new vector<int>();
    pole->resize(featureCount);
    (*pole)[0] = hp;
    (*pole)[1] = distanceFromClosestEnemy;
	(*pole)[2] = marineCount;
    /*(*pole)[3] = vzd1odNepriatela;
    (*pole)[4] = vzd2odNepriatela;
	(*pole)[5] = vzd3odNepriatela;*/
	(*pole)[3] = (*quadrantSafety)[0];
	(*pole)[4] = (*quadrantSafety)[1];
	(*pole)[5] = (*quadrantSafety)[2];
	(*pole)[6] = (*quadrantSafety)[3];
    return pole;
}


int MarineFeature::get_lastAction()
{
	return lastAction;
}
void MarineFeature::set_lastAction(int action)
{
	lastAction = action;
}

float MarineFeature:: get_hpValue()
{
	return hpValue;
}

void MarineFeature::set_weaponCD(float cd)
{
	weaponCD = cd;
}

float MarineFeature::get_weaponCD()
{
	return weaponCD;
}

void MarineFeature::set_weaponCDLastReward(float cd)
{
	weaponCDLastReward = cd;
}

float MarineFeature::get_weaponCDLastReward()
{
	return weaponCDLastReward;
}

void MarineFeature::set_marineCount(int count)
{
	marineCount = count;
}

int MarineFeature::get_marineCount()
{
	return marineCount;
}

