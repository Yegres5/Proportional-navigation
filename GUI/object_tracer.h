#ifndef OBJECT_TRACER_H
#define OBJECT_TRACER_H

#include <QObject>
#include <QVector>
#include <QGraphicsItemGroup>
#include <QPen>

class QGraphicsLineItem;

class tracer : public QObject
{

  Q_OBJECT

private:
  QVector<QGraphicsLineItem*> line;
  const int LineMaxCount;
  int count;
  QGraphicsLineItem* compareItem;
  QGraphicsItemGroup* group;
  QPen* pen;

public:
  tracer(QPen* pen, double lineCount = 50, QObject* parent = nullptr);
  virtual ~tracer(){}

  QGraphicsLineItem* newLine(double x, double y);

signals:
  void deleteElements(QGraphicsItemGroup*);

};

#endif // OBJECT_TRACER_H
