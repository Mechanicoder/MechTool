#include "DrawProxy.h"

#include <TopoDS_Compound.hxx>
#include <TopoDS_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <AIS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>

#include <iostream>


DrawProxy::DrawProxy(Handle(AIS_InteractiveContext) context)
    : context_(context)
{
}

DrawProxy::~DrawProxy()
{
}

void DrawProxy::DrawCurvs(std::vector<Handle(Geom_Curve)> &curves)
{
    TopoDS_Compound compound;
    TopoDS_Builder builder;
    builder.MakeCompound(compound);

    for (size_t i = 0; i < curves.size(); i++)
    {
        BRepBuilderAPI_MakeEdge make_edge(curves[i]);
        if (make_edge.IsDone())
        {
            builder.Add(compound, make_edge.Shape());
        }
    }
    if (!compound.IsNull())
    {
        DrawShape(compound);
        std::cout << "Draw " + std::to_string(curves.size()) + " curves finished!\n";
    }
}

void DrawProxy::DrawPoints(const std::vector<gp_Pnt>& pnts)
{
    TopoDS_Compound compound;
    TopoDS_Builder builder;
    builder.MakeCompound(compound);
    for (size_t i = 0; i < pnts.size(); i++)
    {
        BRepBuilderAPI_MakeVertex make_vertex(pnts[i]);
        if (make_vertex.IsDone())
        {
            builder.Add(compound, make_vertex.Shape());
        }
    }
    if (!compound.IsNull())
    {
        DrawShape(compound);
    }
}

void DrawProxy::DrawShape(const TopoDS_Shape & shape)
{
    Handle(AIS_Shape) obj = new AIS_Shape(shape);
    context_->Display(obj);
}

