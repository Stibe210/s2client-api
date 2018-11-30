#include "qlbot/marine_feature.h"


MarineFeature::MarineFeature() : hp1(0), hp2(0), hp3(0), vzd1odNepriatela(0), vzd2odNepriatela(0), vzd3odNepriatela(0)
{
	quadrantSafety = new int[4] {0, 0, 0, 0};
}

MarineFeature::~MarineFeature()
{
}

void MarineFeature::set_hp1(float hpPer)
{
    //skuska prikladat mensiemu hp velku dolezitost
    if (hpPer < 0.1)
    {
        hp1 = 1;
    }
    else if (hpPer < 0.2)
    {
        hp1 = 2;
    }
	else if (hpPer < 0.3)
	{
		hp1 = 3;
	}
	else if (hpPer < 0.6)
	{
		hp1 = 4;
	}
	else if (hpPer < 0.9)
	{
		hp1 = 5;
	}
	else
	{
		hp1 = 6;
	}
}

int MarineFeature::get_hp1()
{
    return hp1;
}

void MarineFeature::set_hp2(float hpPer)
{
	//skuska prikladat mensiemu hp velku dolezitost
	if (hpPer < 0.1)
	{
		hp2 = 1;
	}
	else if (hpPer < 0.2)
	{
		hp2 = 2;
	}
	else if (hpPer < 0.3)
	{
		hp2 = 3;
	}
	else if (hpPer < 0.6)
	{
		hp2 = 4;
	}
	else if (hpPer < 0.9)
	{
		hp2 = 5;
	}
	else
	{
		hp2 = 6;
	}
}

int MarineFeature::get_hp2()
{
	return hp2;
}

void MarineFeature::set_hp3(float hpPer)
{
	//skuska prikladat mensiemu hp velku dolezitost
	if (hpPer < 0.1)
	{
		hp3 = 1;
	}
	else if (hpPer < 0.2)
	{
		hp3 = 2;
	}
	else if (hpPer < 0.3)
	{
		hp3 = 3;
	}
	else if (hpPer < 0.6)
	{
		hp3 = 4;
	}
	else if (hpPer < 0.9)
	{
		hp3 = 5;
	}
	else
	{
		hp3 = 6;
	}
}

int MarineFeature::get_hp3()
{
	return hp3;
}

void MarineFeature::set_vzd1odNepriatela(float distance)
{
	//treba zistit na aku vzdialenost marinak dosiahne strielat
	if (distance < 2)
	{
		vzd1odNepriatela = 1;
	}
	else if (distance < 3)
	{
		vzd1odNepriatela = 2;
	}
	else if (distance < 4)
	{
		vzd1odNepriatela = 3;
	}
	else if (distance < 4.5)
	{
		vzd1odNepriatela = 4;
	}
	else if (distance < 5)
	{
		vzd1odNepriatela = 5;
	}
	else
	{
		vzd1odNepriatela = 6;
	}
}



int MarineFeature::get_vzd1odNepriatela()
{
	return vzd1odNepriatela;
}



void MarineFeature::set_vzd2odNepriatela(float distance)
{
	//treba zistit na aku vzdialenost marinak dosiahne strielat
	if (distance < 2)
	{
		vzd2odNepriatela = 1;
	}
	else if (distance < 3)
	{
		vzd2odNepriatela = 2;
	}
	else if (distance < 4)
	{
		vzd2odNepriatela = 3;
	}
	else if (distance < 4.5)
	{
		vzd2odNepriatela = 4;
	}
	else if (distance < 5)
	{
		vzd2odNepriatela = 5;
	}
	else
	{
		vzd2odNepriatela = 6;
	}
}

int MarineFeature::get_vzd2odNepriatela()
{
	return vzd2odNepriatela;
}

void MarineFeature::set_vzd3odNepriatela(float distance)
{
	//treba zistit na aku vzdialenost marinak dosiahne strielat
	if (distance < 2)
	{
		vzd3odNepriatela = 1;
	}
	else if (distance < 3)
	{
		vzd3odNepriatela = 2;
	}
	else if (distance < 4)
	{
		vzd3odNepriatela = 3;
	}
	else if (distance < 4.5)
	{
		vzd3odNepriatela = 4;
	}
	else if (distance < 5)
	{
		vzd3odNepriatela = 5;
	}
	else
	{
		vzd3odNepriatela = 6;
	}
}



int MarineFeature::get_vzd3odNepriatela()
{
	return vzd3odNepriatela;
}

void MarineFeature::set_quadrantSafety(float* safety)
{
	//napad: najst min a max hodnotu a vsetky hodnoty vcucnut medzi ne a rozdelit na 4 casti (normalizacia sa to vola?).
	float min = FLT_MAX;
	float max = FLT_MIN;	
	for (int i = 0; i < 4; i++)
	{
		float sft = safety[i];
		if (sft >= max)
			max = sft;
		if (sft <= min)
			min = sft;
	}
	for (int i = 0; i < 4; i++)
	{
		float sft = (safety[i]-min)/max;
		if (sft <= 0.125)
			quadrantSafety[i] = 0;
		else if (sft <= 0.375)
			quadrantSafety[i] = 1;
		else if (sft <= 0.625)
			quadrantSafety[i] = 2;
		else if (sft <= 0.875)
			quadrantSafety[i] = 3;
		else
			quadrantSafety[i] = 4;
	}
}

int* MarineFeature::get_quadrantSafety()
{
	return quadrantSafety;
}


vector<int>* MarineFeature::to_array()
{
    vector<int>* pole = new vector<int>();
    pole->resize(2);
    (*pole)[0] = hp1;
    (*pole)[1] = vzd1odNepriatela;
	/*(*pole)[2] = hp3;
    (*pole)[3] = vzd1odNepriatela;
    (*pole)[4] = vzd2odNepriatela;
	(*pole)[5] = vzd3odNepriatela;
	(*pole)[6] = quadrantSafety[0];
	(*pole)[7] = quadrantSafety[1];
	(*pole)[8] = quadrantSafety[2];
	(*pole)[9] = quadrantSafety[3];*/
    return pole;
}

