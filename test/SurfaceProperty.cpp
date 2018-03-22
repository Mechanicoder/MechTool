#include "SurfaceProperty.h"
#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx> // uvbounds
#include <gp_Dir.hxx>
#include <GeomLProp_SLProps.hxx>


SurfaceProperty::SurfaceProperty()
{
}


SurfaceProperty::~SurfaceProperty()
{
}

std::vector<std::vector<double> > SurfaceProperty::NormalRate(
    const TopoDS_Face & face,
    double &umin, double &umax, double &vmin, double &vmax, 
    bool exchange_uv)
{
    BRepTools::UVBounds(face, umin, umax, vmin, vmax);

    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    GeomLProp_SLProps props(surf, 2, 1e-6);

    int N = 500;

    std::vector<std::vector<gp_Dir> > dirs(N + 1);
    for (int i = 0; i <= N; i++)
    {
        double u = umin + i * (umax - umin) / N;
        
        dirs[i].resize(N + 1);
        for (int j = 0; j <= N; j++)
        {
            double v = vmin + j * (vmax - vmin) / N;
            if (exchange_uv)
            {
                props.SetParameters(v, u);
            }
            else
            {
                props.SetParameters(u, v);
            }

            dirs[i][j] = props.Normal();
        }
    }

    std::vector<std::vector<double> > result(N-1);
    
    // 如何描述法向矢量的波动状态
    for (int i = 1; i < N; i++)
    {
        // 均方差
        result[i - 1].resize(N - 1);
        for (int j = 1; j < N; j++)
        {
            const int M = 2;
            double ang[M];
            ang[0] = dirs[i][j].Angle(dirs[i][j - 1]);
            ang[1] = dirs[i][j].Angle(dirs[i][j + 1]);
            //ang[0] = dirs[i][j].Angle(dirs[i - 1][j]);
            //ang[3] = dirs[i][j].Angle(dirs[i][j + 1]);

            double ang_mean = 0.0;
            for (int m = 0; m < M; m++)
            {
                ang_mean += ang[m];
            }
            ang_mean /= (double)M;

            double sigma = 0.0;;
            for (int  m = 0; m < M; m++)
            {
                sigma += (ang[m] - ang_mean) *(ang[m] - ang_mean);
            }
            result[i - 1][j - 1] = sigma / (double)M;
        }
    }
    return result;
}

std::vector<std::vector<double>> SurfaceProperty::Curvature(const TopoDS_Face & face, double & umin, double & umax, double & vmin, double & vmax, bool exchange_uv)
{
    BRepTools::UVBounds(face, umin, umax, vmin, vmax);

    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    GeomLProp_SLProps props(surf, 2, 1e-6);

    int N = 500;
    std::vector<std::vector<double> > res(N + 1);
    for (int i = 0; i <= N; i++)
    {
        double u = umin + i * (umax - umin) / N;

        res[i].resize(N + 1);
        for (int j = 0; j <= N; j++)
        {
            double v = vmin + j * (vmax - vmin) / N;
            if (exchange_uv)
            {
                props.SetParameters(v, u);
            }
            else
            {
                props.SetParameters(u, v);
            }

            res[i][j] = props.GaussianCurvature();
        }
    }
    return res;
}
