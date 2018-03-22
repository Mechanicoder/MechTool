#pragma once
#include <vector>

#include <gp_Pnt.hxx>
#include <Geom_Surface.hxx>
class CurveTools
{
public:
    CurveTools();
    ~CurveTools();

    static Handle(Geom_Curve) Interpolate(const std::vector<gp_Pnt> &pnts);

    static std::vector<double> SamplesWithTolerance(const Handle(Geom_Curve) &curve, double tole = 0.01);
};

