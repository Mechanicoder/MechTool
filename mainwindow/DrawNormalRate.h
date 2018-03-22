#pragma once

#include <QDialog>
#include <QRgb>
#include <QImage>

#include <vector>

class DrawNormalRate : public QDialog
{
    Q_OBJECT

public:
    DrawNormalRate(const std::vector<std::vector<double> > &rates, 
        double umin, double umax, double vmin, double vmax,
        QWidget *parent = nullptr);

    ~DrawNormalRate();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    void FindMaxRate(const std::vector<std::vector<double> > &rates);
    QRgb RateToRGB(double rate);

    QImage image_;
    QSize size_;
    double maxRate_;

    double umin_, umax_, vmin_, vmax_;
};
