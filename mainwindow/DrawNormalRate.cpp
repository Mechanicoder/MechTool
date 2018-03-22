#include "DrawNormalRate.h"

#include <QImage>
#include <QMouseEvent>

#define _USE_MATH_DEFINES
#include <math.h>

DrawNormalRate::DrawNormalRate(
    const std::vector<std::vector<double> > &rates, 
    double umin, double umax, double vmin, double vmax,
    QWidget *parent)
    : QDialog(parent), umin_(umin), umax_(umax), vmin_(vmin), vmax_(vmax)
{
    this->setMouseTracking(true);

    if (!rates.empty() && !rates.front().empty())
    {
        FindMaxRate(rates);

        int w = rates.size();
        int h = rates.front().size();

        image_ = QImage(w, h, QImage::Format_RGB32);
        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                image_.setPixel(j, i, RateToRGB(rates[i][j]));
            }
        }
        image_ = image_.scaled(w << 1, h << 1);

        image_.save("F:/a.jpg");

        this->resize(w * 1.5, h * 1.5);
        QPalette palette;
        palette.setBrush(QPalette::Background, QBrush(image_));
        this->setPalette(palette);

        size_ = this->size();
    }
}

DrawNormalRate::~DrawNormalRate()
{
}

void DrawNormalRate::mouseMoveEvent(QMouseEvent * event)
{
    if (umax_ - umin_ > 0 && vmax_ - vmin_ > 0)
    {
        QPoint p = event->pos();
        double u = (double)p.x() / width() * (umax_ - umin_) + umin_;
        double v = (double)p.y() / height() * (vmax_ - vmin_) + vmin_;

        this->setWindowTitle(QString("UV: %1, %2").arg(u).arg(v));
    }
}

void DrawNormalRate::paintEvent(QPaintEvent * event)
{
    if (size_ != size()) // QImage 频繁刷新狂占CPU
    {
        QPalette palette;
        palette.setBrush(QPalette::Background, QBrush(image_.scaled(this->width(), this->height())));
        this->setPalette(palette);

        size_ = size();
    }
    // 0.984591--0.4706
}

void DrawNormalRate::FindMaxRate(const std::vector<std::vector<double>>& rates)
{
    maxRate_ = -1;
    for (int i = 0; i < rates.size(); i++)
    {
        for (int j = 0; j < rates[i].size(); j++)
        {
            if (rates[i][j] > maxRate_)
            {
                maxRate_ = rates[i][j];
            }
        }
    }
}

QRgb DrawNormalRate::RateToRGB(double rate)
{
    // 以 1 度为基准
    // 0--白色，1--黑色；颜色越深表示法向夹角越大
    double ratio = 1 - rate / maxRate_;
    int rgb = ratio * 255;
    return qRgb(rgb, rgb, rgb);
}
