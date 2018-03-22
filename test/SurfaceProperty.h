#pragma once

#include "comn.h"

#include <TopoDS_Face.hxx>

#include <vector>

class TEST_API SurfaceProperty
{
public:
    SurfaceProperty();
    ~SurfaceProperty();

    // 求解曲面法向，默认两个方向上的离散点数根据曲面大小确定
    static std::vector<std::vector<double> > NormalRate(
        const TopoDS_Face &face, 
        double &umin, double &umax, double &vmin, double &vmax,
        bool exchange_uv = false);

    static std::vector<std::vector<double> > Curvature(
        const TopoDS_Face &face,
        double &umin, double &umax, double &vmin, double &vmax,
        bool exchange_uv = false);
};

