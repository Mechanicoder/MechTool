#include "CurveTools.h"

#include <TColgp_HArray1OfPnt.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>

#include <GeomAdaptor_Curve.hxx>

CurveTools::CurveTools()
{
}


CurveTools::~CurveTools()
{
}

Handle(Geom_Curve) CurveTools::Interpolate(const std::vector<gp_Pnt>& pnts)
{
    size_t size = pnts.size();
    Handle(TColgp_HArray1OfPnt) array_pnts = new TColgp_HArray1OfPnt(1, size);
    for (size_t i = 0; i < pnts.size(); ++i)
    {
        array_pnts->SetValue(i + 1, pnts[i]);
    }
    GeomAPI_Interpolate interpolate(array_pnts, false, 1e-7);
    interpolate.Perform();
    if (interpolate.IsDone())
    {
        return interpolate.Curve();
    }
    return Handle(Geom_Curve)();
}

std::vector<double> CurveTools::SamplesWithTolerance(const Handle(Geom_Curve)& curve, double tolerance)
{
    GeomAdaptor_Curve adaptor(curve);
    GCPnts_QuasiUniformDeflection uniform(adaptor, tolerance);

    std::vector<double> ret;
    if (uniform.IsDone())
    {
        int num = uniform.NbPoints();
        ret.resize(num);
        for (int i = 0; i < num; i++)
        {
            ret[i] = uniform.Parameter(i + 1);
        }
    }
    return ret;
}
