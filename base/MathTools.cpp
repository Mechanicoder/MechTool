#include "MathTools.h"



MathTools::MathTools()
{
}


MathTools::~MathTools()
{
}

VecDs MathTools::SamplesCC(double start, double end, size_t num)
{
    std::vector<double> ret;
    if (num == 1)
    {
        ret.emplace_back(end);
    }
    else
    {
        ret.resize(num);
        ret[0] = start;
        double offset = (end - start) / double(num - 1);
        for (size_t i = 1; i < num; ++i)
        {
            ret[i] = offset + ret[i - 1];
        }
    }
    return ret;
}
