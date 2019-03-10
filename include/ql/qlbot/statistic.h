#pragma once
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "circular_buffer.h"

using namespace std;

class Statistic
{
protected:
    void copy(Statistic& other);
    void copy(const Statistic& other);
    bool is_cont;
    circular_buffer<double>* batch;
    int node_counter = 0;
    double sum = 0;
    double pow_sum = 0;
    int count_ = 0;
    int save_after;

public:
    Statistic(int save_after, bool is_cont = false, int batch_size = 1);
    virtual ~Statistic();
    Statistic(Statistic& other);
    Statistic(const Statistic& other);
    Statistic& operator=(Statistic& other);
    Statistic& operator=(const Statistic& other);

    void reset();
    double mean();
    double variance();
    double std();
    virtual void add(double value);
    virtual int count();

    string to_csv_string();

};
