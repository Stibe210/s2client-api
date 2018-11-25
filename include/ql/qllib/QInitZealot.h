#pragma once
#include "QInit.h"

class QInitZealot: public QInit
{
public:
    QInitZealot();
    ~QInitZealot();
    void initQ(vector<float>* qHodnoty) override;
};

