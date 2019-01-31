#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>
#include <QDebug>
#include <typeinfo>
#include <QPainter>
#include <QString>
#include <QList>
#include <QSizePolicy>
#include <QPen>
#include <QtMath>

#include "graphics_view_zoom.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//Editing scene
    QGraphicsScene* scene = new QGraphicsScene(ui->graphicsView_XY);
    scene->setSceneRect(ui->graphicsView_XY->geometry());
    scene->setBackgroundBrush(Qt::white);
    ui->graphicsView_XY->setScene(scene);
    ui->graphicsView_XY->scale(0.01, 0.01);
    Graphics_view_zoom* z = new Graphics_view_zoom(ui->graphicsView_XY);
    z->set_modifiers(Qt::NoModifier);

    scene = new QGraphicsScene(ui->graphicsView_XZ);
    scene->setSceneRect(ui->graphicsView_XZ->geometry());
    scene->setBackgroundBrush(Qt::white);
    ui->graphicsView_XZ->setScene(scene);
    ui->graphicsView_XZ->scale(0.01, 0.01);
    z = new Graphics_view_zoom(ui->graphicsView_XZ);
    z->set_modifiers(Qt::NoModifier);


    scene = new QGraphicsScene(ui->graphicsView_ZY);
    scene->setSceneRect(ui->graphicsView_ZY->geometry());
    scene->setBackgroundBrush(Qt::white);
    ui->graphicsView_ZY->setScene(scene);
    ui->graphicsView_ZY->scale(0.01, 0.01);
    z = new Graphics_view_zoom(ui->graphicsView_ZY);
    z->set_modifiers(Qt::NoModifier);

//Editing dataTable
    ui->dataTable->setRowCount(9);
    ui->dataTable->setColumnCount(2);
    ui->dataTable->setFixedSize(ui->dataTable->horizontalHeader()->length()+ui->dataTable->verticalHeader()->width(),
                       ui->dataTable->verticalHeader()->length()+ui->dataTable->horizontalHeader()->height());
    ui->dataTable->setItem(0, 0, new QTableWidgetItem("X"));
    ui->dataTable->setItem(1, 0, new QTableWidgetItem("Y"));
    ui->dataTable->setItem(2, 0, new QTableWidgetItem("Z"));
    ui->dataTable->setItem(3, 0, new QTableWidgetItem("V"));
    ui->dataTable->setItem(4, 0, new QTableWidgetItem("teta"));
    ui->dataTable->setItem(5, 0, new QTableWidgetItem("psi"));
    ui->dataTable->setItem(6, 0, new QTableWidgetItem("gamma"));
    ui->dataTable->setItem(7, 0, new QTableWidgetItem("n_y"));
    ui->dataTable->setItem(8, 0, new QTableWidgetItem("n_x"));

//Editing scroll items
    dX = 0;
    dY = 0;
    dZ = 0;

    Zoom_X = Zoom_Y = Zoom_Z = 1;

//Start flag
    flag = 0;


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createHash(LA* flyObject)
{

    QPen* pen = new QPen();
    pen->setWidth(100);

    if(flyObject->objectName() == "Rocket"){
        pen->setColor(Qt::blue);
    }else{
        pen->setColor(Qt::red);
    }

    tracer* tracerXZ = new tracer(pen, 100, ui->graphicsView_XZ);

    connect(tracerXZ, SIGNAL(deleteElements(QGraphicsItemGroup*)),
            ui->graphicsView_XZ, SLOT(destroyGroupOfItems(QGraphicsItemGroup*)));

    drawingObject* Obj_XY = new drawingObject(pen->brush(), "XY");
    drawingObject* Obj_XZ = new drawingObject(pen->brush(), "XZ");
    drawingObject* Obj_ZY = new drawingObject(pen->brush(), "ZY");

    drawingObjects_XY.insert(flyObject,Obj_XY);
    drawingObjects_XZ.insert(flyObject,Obj_XZ);
    drawingObjects_ZY.insert(flyObject,Obj_ZY);

    tracer_XZ.insert(Obj_XZ, tracerXZ);

    ui->graphicsView_XY->scene()->addItem(Obj_XY);
    ui->graphicsView_XZ->scene()->addItem(Obj_XZ);
    ui->graphicsView_ZY->scene()->addItem(Obj_ZY);

    Obj_XY->setPos((flyObject->getX()+dX)/Zoom_X,(-flyObject->getY()-dY)/Zoom_Y);
    Obj_XZ->setPos((flyObject->getX()+dX)/Zoom_X,(flyObject->getZ()+dZ)/Zoom_Z);
    Obj_ZY->setPos((flyObject->getZ()+dZ)/Zoom_Z,(-flyObject->getY()-dY)/Zoom_Y);

    Obj_XY->scene()->setSceneRect(Obj_XY->scene()->itemsBoundingRect());
    Obj_XZ->scene()->setSceneRect(Obj_XZ->scene()->itemsBoundingRect());
    Obj_ZY->scene()->setSceneRect(Obj_ZY->scene()->itemsBoundingRect());

    Obj_XY->show();
    Obj_XZ->show();
    Obj_ZY->show();

    setTableData(flyObject);
}

//TODO: Thread pool for graphic in MainWindow::redraw.
//      Error: QObject::startTimer: Timers cannot be started from another thread.
//             ASSERT: "unindexedItems.isEmpty()" in file graphicsview\qgraphicsscenebsptreeindex.cpp, line 387;


void MainWindow::reDraw(const QVector<LA *> *objects)
{
  double x(objects->last()->getX()),
         w(objects->last()->getX());

  double y(objects->last()->getY()),
         h(objects->last()->getY());

  double z(objects->last()->getZ()),
         l(objects->last()->getZ());

    for(QVector<LA*>::const_iterator iter(objects->begin());
        iter != objects->end();
        iter++){

        if (x > (*iter)->getX())
          x = (*iter)->getX();

        if (y < (*iter)->getY())
          y = (*iter)->getY();

        if (z > (*iter)->getZ())
          z = (*iter)->getZ();

        if (w < (*iter)->getX())
          w = (*iter)->getX();

        if (h > (*iter)->getY())
          h = (*iter)->getY();

        if (l < (*iter)->getZ())
          l = (*iter)->getZ();

        drawObjectOnWindow(*iter);
        setTableData(*iter);
    }

    w = fabs(x - w);
    h = fabs(y - h);
    l = fabs(z - l);

    ui->graphicsView_XY->scene()->setSceneRect(x,-y,w,h);
    ui->graphicsView_ZY->scene()->setSceneRect(z,-y,l,h);
    ui->graphicsView_XZ->scene()->setSceneRect(x,z,w,l);

    if(flag)
        emit DrawEnd();
}

void MainWindow::on_pushButton_clicked()
{
    flag = !flag;
    emit DrawEnd();
}

void MainWindow::drawObjectOnWindow(LA *flyObject)
{
    drawingObject* obj_XY = drawingObjects_XY.value(flyObject);
    drawingObject* obj_XZ = drawingObjects_XZ.value(flyObject);
    drawingObject* obj_ZY = drawingObjects_ZY.value(flyObject);

    //tracer* obj_tracer_XY = tracer_XY.value(obj_XY);
    tracer* obj_tracer_XZ = tracer_XZ.value(obj_XZ);

    //tracer* obj_tracer_XZ = tracer_XZ.value(obj_XZ);

    QPen* pen = new QPen();
    pen->setWidth(100);
    if (flyObject->objectName() == "Rocket"){
        pen->setColor(Qt::blue);
    }else{
        pen->setColor(Qt::red);
    }

    ui->graphicsView_XY->scene()->addLine(obj_XY->x(),
                                          obj_XY->y(),
                                          ((flyObject)->getX()+dX)/Zoom_X,
                                          (-(flyObject)->getY()-dY)/Zoom_Y,
                                          *pen);

    obj_XY->setPos(((flyObject)->getX()+dX)/Zoom_X,(-(flyObject)->getY()-dY)/Zoom_Y);
//    ui->graphicsView_XZ->scene()->addLine(obj_XZ->x(),
//                                          obj_XZ->y(),
//                                          ((flyObject)->getX()+dX)/Zoom_X,
//                                          ((flyObject)->getZ()+dZ)/Zoom_Z,
//                                          *pen);



    ui->graphicsView_XZ->scene()->addItem(
          obj_tracer_XZ->newLine(((flyObject)->getX()+dX)/Zoom_X,
                                 ((flyObject)->getZ()+dZ)/Zoom_Z));

    obj_XZ->setPos(((flyObject)->getX()+dX)/Zoom_X,((flyObject)->getZ()+dZ)/Zoom_Z);

    ui->graphicsView_ZY->scene()->addLine(obj_ZY->x(),
                                          obj_ZY->y(),
                                          ((flyObject)->getZ()+dZ)/Zoom_Z,
                                          (-(flyObject)->getY()-dY)/Zoom_Y,
                                          *pen);

    obj_ZY->setPos(((flyObject)->getZ()+dZ)/Zoom_Z,(-(flyObject)->getY()-dY)/Zoom_Y);
}

void MainWindow::setTableData(LA *flyObject)
{
    //double temp = ui->dataTable->column();
    ui->dataTable->setItem(0,1,new QTableWidgetItem(QString("%1").arg(flyObject->getX())));
    ui->dataTable->setItem(1,1,new QTableWidgetItem(QString("%1").arg(flyObject->getY())));
    ui->dataTable->setItem(2,1,new QTableWidgetItem(QString("%1").arg(flyObject->getZ())));
    ui->dataTable->setItem(3,1,new QTableWidgetItem(QString("%1").arg(flyObject->GetV())));
    ui->dataTable->setItem(4,1,new QTableWidgetItem(QString("%1").arg(flyObject->getTeta().getValue())));
    ui->dataTable->setItem(5,1,new QTableWidgetItem(QString("%1").arg(flyObject->getPsi().getValue())));
    ui->dataTable->setItem(6,1,new QTableWidgetItem(QString("%1").arg(flyObject->getGamma().getValue())));
    ui->dataTable->setItem(7,1,new QTableWidgetItem(QString("%1").arg(flyObject->GetNy())));
    ui->dataTable->setItem(8,1,new QTableWidgetItem(QString("%1").arg(flyObject->GetNx())));
}

void MainWindow::reDrawAllViews()
{
    QVector<QList<drawingObject*>> vec;

    vec.push_back(*(new QList<drawingObject*>(drawingObjects_XY.values())));
    vec.push_back(*(new QList<drawingObject*>(drawingObjects_XZ.values())));
    vec.push_back(*(new QList<drawingObject*>(drawingObjects_ZY.values())));

//    double dX_;
//    double dY_;

//    double Zoom_X_;
//    double Zoom_Y_;

    for(QVector<QList<drawingObject*>>::iterator vecIter(vec.begin());
       vecIter<vec.end();
       vecIter++){
        for (QList<drawingObject*>::iterator iter(vecIter->begin());
             iter<vecIter->end();
             iter++){
            drawingObject* obj = *iter;

            obj->setPos(obj->x(), obj->y());

        }
    }
}

void MainWindow::on_ScaleButton_clicked()
{
    reDrawAllViews();
}

