#include "qlbot/statistic_cont.h"

StatisticConst::StatisticConst(int save_after, int batch_size): Statistic(save_after)
{
    this->batch_size = batch_size;
}

StatisticConst::~StatisticConst()
{
}

void override StatisticConst::add(double value)
{
    
}
