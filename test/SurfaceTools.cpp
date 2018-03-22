#include "SurfaceTools.h"

#include "../base/MathTools.h"
#include "../test/CurveTools.h"

#include "../WComn/wtypedef.h"

#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <GeomLProp_SLProps.hxx>


SurfaceTools::SurfaceTools()
{
}

SurfaceTools::~SurfaceTools()
{
}

std::vector<Handle(Geom_Curve)> SurfaceTools::OffsetCurves(
    const Handle(Geom_Surface) &surf,
    double dist,
    std::vector<std::vector<gp_Pnt> > &pnts)
{
    double u1, u2, v1, v2;
    surf->Bounds(u1, u2, v1, v2);

    GeomLProp_SLProps props(surf, 1, 1e-7);

    Handle(Geom_Curve) viso = surf->VIso((v1 + v2) / 2.0);
    VecDs us = CurveTools::SamplesWithTolerance(viso, 0.0005);

    Handle(Geom_Curve) uiso = surf->UIso((u1 + u2) / 2.0);
    VecDs vs = CurveTools::SamplesWithTolerance(uiso, 0.0005);

    std::vector<Handle(Geom_Curve)> curves(vs.size());
    for (size_t v = 0; v < vs.size(); v++)
    {
        std::vector<gp_Pnt> interp_pnts(us.size());
        for (size_t u = 0; u < us.size(); u++)
        {
            props.SetParameters(us[u], vs[v]);
            gp_Dir normal = props.Normal();

            interp_pnts[u] = props.Value().Translated(gp_Vec(normal) * dist);
        }
        curves[v] = CurveTools::Interpolate(interp_pnts);
        pnts.emplace_back(interp_pnts);
    }
    return curves;
}
