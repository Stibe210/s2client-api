#pragma once
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
using namespace std;
class MarineFeature
{
private:
    int hp;
    int distanceFromClosestEnemy;
    int* quadrantSafety;
    int featureCount;
    int lastAction;
    float hpValue;
    float weaponCD;
    float weaponCDLastReward;

public:
    
    MarineFeature(int);
    MarineFeature();
    ~MarineFeature();

    void set_hp(float hpPer, float hp);
    int get_hp();
    void set_distanceFromClosestEnemy(float distance);
    int get_distanceFromClosestEnemy();
    void set_quadrantSafety(float* safety);
    int* get_quadrantSafety();
    float get_hpValue();
    void set_lastAction(int);
    int get_lastAction();

    float get_weaponCD();
    void set_weaponCD(float);

    float get_weaponCDLastReward();
    void set_weaponCDLastReward(float);

    vector<int>* to_array();

};