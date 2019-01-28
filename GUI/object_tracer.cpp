#include "object_tracer.h"

#include <QVector>
#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QPen>

tracer::tracer(QPen* pen, double lineCount, QObject* parent)
  : QObject(parent),
    line(0),
    LineMaxCount(lineCount),
    count(0),
    compareItem(nullptr),
    pen(pen)
{
  group = new QGraphicsItemGroup();
}

QGraphicsLineItem* tracer::newLine(double x, double y)
{
  if (!count){
      QGraphicsLineItem* item = new QGraphicsLineItem(x,y,x,y);
      item->setPen(*pen);
      line.push_back(item);
      compareItem = item;
      count++;
    }else{
      QGraphicsLineItem* item = new QGraphicsLineItem(line.last()->line().p2().x(),
                                                 line.last()->line().p2().y(),
                                                 x,y);
      item->setPen(*pen);
      line.push_back(item);
      count++;
    }

//  if (count == 100){
//      QGraphicsLineItem* item = new QGraphicsLineItem(compareItem->line().p1().x(),
//                                                     compareItem->line().p1().y(),
//                                                     line.last()->line().p2().x(),
//                                                     line.last()->line().p2().y());
//      item->setPen(*pen);
//      line.push_back(item);

//      for (QVector<QGraphicsLineItem*>::reverse_iterator iter(line.rbegin()+1);
//           iter!=(line.rbegin()+101);
//           iter++){
//          QGraphicsLineItem* it = *iter;
//          group->addToGroup(it);
//        }
//      emit deleteElements(group);

//      line.remove(line.indexOf(compareItem),100);
//      count -= 100;
//    }



  return line.last();
}




