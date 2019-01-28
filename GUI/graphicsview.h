#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsItemGroup>

class graphicsView : public QGraphicsView
{
  Q_OBJECT
private:
  bool pressed;

  int m_originX;
  int m_originY;

protected:
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);

public:
  graphicsView(QWidget* parent = nullptr);
  virtual ~graphicsView(){}

public slots:
  void destroyGroupOfItems(QGraphicsItemGroup* group);


};

#endif // GRAPHICSVIEW_H
