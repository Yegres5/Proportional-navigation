#include "graphicsview.h"
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsScene>
void graphicsView::mouseMoveEvent(QMouseEvent* event)
{
  if (pressed){
      QPointF newP = mapToScene(event->pos());
      QPointF translation = newP;// - oldp;

      translate(translation.x(),translation.y());

      //centerOn(translation);


      m_originX = event->x();
      m_originY = event->y();
    }
}

void graphicsView::mousePressEvent(QMouseEvent* event)
{
  pressed = true;
  m_originX = event->x();
  m_originY = event->y();
}

void graphicsView::mouseReleaseEvent(QMouseEvent* event)
{
  pressed = false;
}

graphicsView::graphicsView(QWidget* parent)
  :QGraphicsView(parent),pressed(false)
{
  setMouseTracking(true);
  //scene()->setSceneRect(scene->itemsBoundingRect());

}

void graphicsView::destroyGroupOfItems(QGraphicsItemGroup* group)
{
  scene()->destroyItemGroup(group);
}
