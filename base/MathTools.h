#pragma once
#include "comn.h"
#include "../WComn/wtypedef.h"

class BASE_API MathTools
{
public:
    MathTools();
    ~MathTools();

    static VecDs SamplesCC(double start, double end, size_t num);
};

