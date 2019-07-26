#pragma once

#include <QColor>
#include <QWidget>

class DisplayWidget : public QWidget
{
public:
    DisplayWidget(unsigned char arr[64 * 32]);
    ~DisplayWidget();

    void setResolution(int r);
    int getResolution();
    void setBgColor(QColor color);
    void setFgColor(QColor color);
    QColor getBgColor();
    QColor getFgColor();
    void clear();

    unsigned char* screen;

    unsigned char* to2d(unsigned char* screenarray, int pointx, int pointy);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int resolution;

    QColor bgColor;
    QColor fgColor;
};
