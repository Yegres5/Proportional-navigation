#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
//#include <QThreadPool>
#include <vector>

#include "la.h"
#include "drawingobject.h"
#include "object_tracer.h"

#define PROGRAMM_LOOPED 1
#define CORRECT_X 2
#define CORRECT_Y 4
#define CORRECT_Z 8
#define CORRECT_X_SCALE 16
#define CORRECT_Y_SCALE 32
#define CORRECT_Z_SCALE 64


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createHash(LA *flyObject);
signals:
    void DrawEnd();
public slots:
    void reDraw(const QVector<LA *> *objects);
private slots:
    void on_pushButton_clicked();
    void on_ScaleButton_clicked();
private:

    Ui::MainWindow *ui;
    QHash<LA*,drawingObject*> drawingObjects_XY;
//    QHash<drawingObject*, tracer*> tracer_XY;
    QHash<LA*,drawingObject*> drawingObjects_XZ;
    QHash<drawingObject*, tracer*> tracer_XZ;
    QHash<LA*,drawingObject*> drawingObjects_ZY;
    double dX,dY,dZ;
    double Zoom_X,Zoom_Y,Zoom_Z;

//    QThreadPool* threadPool;

    void drawObjectOnWindow(LA* flyObject);
    void setTableData(LA* flyObject);
    void reDrawAllViews();

    unsigned char flag;
};

#endif // MAINWINDOW_H
