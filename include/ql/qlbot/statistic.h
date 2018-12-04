#pragma once
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"

using namespace std;

class Statistic
{
private:
    void copy(Statistic& other);
    void copy(const Statistic& other);
    int node_counter = 0;
    double sum = 0;
    double pow_sum = 0;
    int count_ = 0;
    vector<Statistic*> values{};
    int save_after;
public:
    Statistic(int save_after);
    ~Statistic();
    Statistic(Statistic& other);
    Statistic(const Statistic& other);
    Statistic& operator=(Statistic& other);
    Statistic& operator=(const Statistic& other);

    void reset();
    double mean();
    double variance();
    double std();
    void add(double value);
    int count();

    string to_csv_string();

};
