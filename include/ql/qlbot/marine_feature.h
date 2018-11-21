#pragma once
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
using namespace std;
class MarineFeature
{
private:
    int hp1;
    int hp2;
    int hp3;
    int vzd1odNepriatela;
    int vzd2odNepriatela;
    int vzd3odNepriatela;
    int* quadrantSafety;

public:
    MarineFeature();
    ~MarineFeature();

    void set_hp1(float hpPer);
    int get_hp1();
    void set_hp2(float hpPer);
    int get_hp2();
    void set_hp3(float hpPer);
    int get_hp3();
    void set_vzd1odNepriatela(float distance);
    int get_vzd1odNepriatela();
    void set_vzd2odNepriatela(float distance);
    int get_vzd2odNepriatela();
    void set_vzd3odNepriatela(float distance);
    int get_vzd3odNepriatela();
    void set_quadrantSafety(float* safety);
    int* get_quadrantSafety();

    vector<int>* to_array();

};