#include <QPainter>
#include "displaywidget.h"

DisplayWidget::DisplayWidget(unsigned char arr[64 * 32])
{
    screen = arr;
    clear();
    repaint();
}

DisplayWidget::~DisplayWidget()
{
}

void DisplayWidget::setResolution(int r)
{
    resolution = r;
}

int DisplayWidget::getResolution()
{
    return resolution;
}

void DisplayWidget::setBgColor(QColor color)
{
    bgColor = color;
}

void DisplayWidget::setFgColor(QColor color)
{
    fgColor = color;
}

QColor DisplayWidget::getBgColor()
{
    return bgColor;
}

QColor DisplayWidget::getFgColor()
{
    return fgColor;
}

void DisplayWidget::clear()
{
    for (int i = 0; i < 2048; i++)
    {
        screen[i] = 0;
    }
}

void DisplayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Qt4CompatiblePainting);
    painter.scale(resolution, resolution);
    QBrush brush(bgColor, Qt::SolidPattern);
    painter.fillRect(0, 0, 64, 32, brush);

    painter.setPen(fgColor);
    painter.setBrush(fgColor);

    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 32; y++)
        {
            if (screen[x + (y * 64)] == 1)
            {
                if (resolution > 1)
                {
                    painter.drawRect(x, y, 1, 1);
                }
                else
                {
                    painter.drawPoint(x, y);
                }
            }
        }
    }
}


