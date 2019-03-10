#pragma once
#include "qlbot/circular_buffer.h"
#include "statistic.h"

class StatisticConst : public Statistic
{

private:
    int batch_size;

public:
    explicit StatisticConst(int save_after)
        : Statistic(save_after)
    {
    }

    explicit StatisticConst(Statistic& other)
        : Statistic(other)
    {
    }

    explicit StatisticConst(const Statistic& other)
        : Statistic(other)
    {
    }
    StatisticConst(int save_after, int batch_size);
    ~StatisticConst();
};
