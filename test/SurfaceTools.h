#pragma once
#include "comn.h"
#include <vector>

#include <Standard_Handle.hxx>
#include <Geom_Curve.hxx>
class Geom_Surface;

class TEST_API SurfaceTools
{
public:
    SurfaceTools();
    ~SurfaceTools();

    static std::vector<Handle(Geom_Curve)> OffsetCurves(
        const Handle(Geom_Surface) &surf, 
        double dist,
        std::vector<std::vector<gp_Pnt> > &pnts);
};

