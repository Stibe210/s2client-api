#pragma once
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
using namespace std;
class MarineFeature
{
private:
    int hp;
    int marineCount;
    int distanceFromClosestEnemy;
    vector<int> *quadrantSafety;
    int featureCount;
    int lastAction;
    float hpValue;
    float weaponCD;
    float weaponCDLastReward;

public:
    
    MarineFeature(int);
    MarineFeature();
    ~MarineFeature();

    void set_marineCount(int count);
    int get_marineCount();
    void set_hp(float, float);
    int get_hp();
    void set_distanceFromClosestEnemy(float distance);
    int get_distanceFromClosestEnemy();
    void set_quadrantSafety(vector<float>* safety);
    vector<int>* get_quadrantSafety();
    float get_hpValue();
    void set_lastAction(int);
    int get_lastAction();

    float get_weaponCD();
    void set_weaponCD(float);

    float get_weaponCDLastReward();
    void set_weaponCDLastReward(float);

    vector<int>* to_array();

};