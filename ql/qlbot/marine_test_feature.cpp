#include "qlbot/marine_test_feature.h"

void MarineTestFeature::copy(MarineTestFeature& other)
{
    hp = other.hp;
    enemy_dist = other.enemy_dist;
}

void MarineTestFeature::copy(const MarineTestFeature& other)
{
    hp = other.hp;
    enemy_dist = other.enemy_dist;
}

MarineTestFeature::MarineTestFeature()
{
}

MarineTestFeature::~MarineTestFeature()
{
}

MarineTestFeature::MarineTestFeature(MarineTestFeature& other)
{
    copy(other);
}

MarineTestFeature::MarineTestFeature(const MarineTestFeature& other)
{
    copy(other);
}

MarineTestFeature& MarineTestFeature::operator=(MarineTestFeature& other)
{
    copy(other);
    return *this;
}

MarineTestFeature& MarineTestFeature::operator=(const MarineTestFeature& other)
{
    copy(other);
    return *this;
}

void MarineTestFeature::set_hp(float hpPer)
{
    if (hpPer < 0.2)
    {
        hp = 1;
    }
    else if (hpPer < 0.35)
    {
        hp = 2;
    }
    else if (hpPer < 0.55)
    {
        hp = 3;
    }
    else if (hpPer < 0.85)
    {
        hp = 4;
    }
    else
    {
        hp = 5;
    }
}

int MarineTestFeature::get_hp()
{
    return hp;
}

void MarineTestFeature::set_enemy_dist(float distance)
{
    if (distance < 2)
    {
        enemy_dist = 1;
    }
    else if (distance < 3)
    {
        enemy_dist = 2;
    }
    else if (distance < 4.5)
    {
        enemy_dist = 3;
    }
    else if (distance < 6)
    {
        enemy_dist = 4;
    }
    else
    {
        enemy_dist = 5;
    }
}

int MarineTestFeature::get_enemy_dist()
{
    return enemy_dist;
}

vector<int>* MarineTestFeature::to_array()
{
    vector<int>* pole = new vector<int>();
    pole->resize(2);
    (*pole)[0] = hp;
    (*pole)[1] = enemy_dist;
    return  pole;
}
