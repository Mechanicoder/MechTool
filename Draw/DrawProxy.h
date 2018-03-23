#ifndef DRAWPROXY_H
#define DRAWPROXY_H

#include "comn.h"

#include <vector>

#include <Standard_Handle.hxx>
#include <gp_Pnt.hxx>
class Geom_Curve;
class AIS_InteractiveContext;
class TopoDS_Shape;

class DRAW_API DrawProxy
{
public:
    DrawProxy(Handle(AIS_InteractiveContext) context);
    ~DrawProxy();
    void DrawCurvs(std::vector<Handle(Geom_Curve)> &curves);

    void DrawPoints(const std::vector<gp_Pnt> &pnts);

    void DrawShape(const TopoDS_Shape &shape);
private:
    Handle(AIS_InteractiveContext) context_;
};

#endif // DRAWPROXY_H

