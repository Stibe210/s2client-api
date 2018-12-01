#pragma once
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"

using namespace std;

class MarineTestFeature
{
private:
    int hp;
    int enemy_dist;
    void copy(MarineTestFeature& other);
    void copy(const MarineTestFeature& other);
public:
    MarineTestFeature();
    ~MarineTestFeature();
    MarineTestFeature(MarineTestFeature& other);
    MarineTestFeature(const MarineTestFeature& other);
    MarineTestFeature& operator=(MarineTestFeature& other);
    MarineTestFeature& operator=(const MarineTestFeature& other);

    void set_hp(float hpPer);
    int get_hp();
    void set_enemy_dist(float distance);
    int get_enemy_dist();

    vector<int>* to_array();
};