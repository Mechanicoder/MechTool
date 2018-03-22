#pragma once
#include <vector>

#include <Standard_Handle.hxx>
class Geom_Curve;

class MainDraw
{
public:
    MainDraw();
    ~MainDraw();

    static void DrawCurvs(std::vector<Handle(Geom_Curve)> &curves);
};

