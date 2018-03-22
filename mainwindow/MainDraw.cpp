#include "MainDraw.h"

#include <TopoDS_Compound.hxx>
#include <TopoDS_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

MainDraw::MainDraw()
{
}


MainDraw::~MainDraw()
{
}

void MainDraw::DrawCurvs(std::vector<Handle(Geom_Curve)> &curves)
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
        
    }
}