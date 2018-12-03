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

public:
    
    MarineFeature(int);
    MarineFeature();
    ~MarineFeature();

    void set_hp(float hpPer);
    int get_hp();
    void set_distanceFromClosestEnemy(float distance);
    int get_distanceFromClosestEnemy();
    void set_quadrantSafety(float* safety);
    int* get_quadrantSafety();

    vector<int>* to_array();

};