#include "drawingobject.h"

#include<QPainter>


drawingObject::drawingObject(QBrush color, const char* axes):
  color(color),
  axes(axes),
  r(1000)
{}

QRectF drawingObject::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-r/2 - penWidth / 2, -r/2 - penWidth / 2,
                   r*2 + penWidth, r*2 + penWidth);
}

void drawingObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    painter->setBrush(color);
    painter->drawEllipse(-r/2,-r/2,r,r);
}

const QBrush drawingObject::getColor() const
{
  return color;
}

const char* drawingObject::getAxes()
{
  return axes;
}
