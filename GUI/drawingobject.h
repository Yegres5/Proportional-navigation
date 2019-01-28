#ifndef DRAWINGOBJECT_H
#define DRAWINGOBJECT_H

#include <QObject>
#include <QGraphicsItem>
#include <QBrush>

#include "la.h"

class drawingObject : public QGraphicsItem
{
private:
    QBrush color;
    const char* axes;
    double r;
public:
    drawingObject(QBrush color, const char *axes);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget);

    inline const QBrush getColor() const;
    const char* getAxes();
};

#endif // DRAWINGOBJECT_H
