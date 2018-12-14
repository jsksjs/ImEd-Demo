#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glsheet.h"
#include "optiondialog.h"
#include "layerbutton.h"

#include <QColorDialog>
#include <QDockWidget>
#include <QListWidget>
#include <QLabel>
#include <QMainWindow>
#include <QActionGroup>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QButtonGroup>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void reinitializeCanvas(const QSize size = QSize(256, 256));

private slots:
    void on_toolDock_dockLocationChanged(const Qt::DockWidgetArea &area);

    void on_optionDock_dockLocationChanged(const Qt::DockWidgetArea &area);

    void on_colorDock_dockLocationChanged(const Qt::DockWidgetArea &area);

    void on_layerDock_dockLocationChanged(const Qt::DockWidgetArea &area);

    void center();
    void left();
    void right();
    void fit();
    void original();

    void showEvent(QShowEvent *event);

    void on_colPrimary_released();
    void on_colSecondary_released();

    void on_colorDock_topLevelChanged(bool topLevel);

    void dialogNew();

    void createCanvas(const QSize size = QSize(256, 256), const QPoint pos = QPoint(0,0));

    void grid(const bool e);

    void on_New_released();

    void selectLayer(QAbstractButton *l);

    void on_Del_released();

    void on_Dup_released();

    void on_Up_released();

    void on_Dw_released();

private:
    Ui::MainWindow *ui;

    QColorDialog *primary, *secondary;
    QColor pColor, sColor;

    QLabel *mouseLocation;
    QLabel *frames;

    GLSheet *canvas;

    QList<QDockWidget*> docks;
    QList<QWidget*> bars;

    QActionGroup *layoutMenu;

    QDoubleSpinBox *zoomPercent;

    QButtonGroup *layerBtns;
    LayerButton *lastChecked;
    int numLayers = 0;

    QPoint last;

    OptionDialog *option;

    QVector4D colorToNVecF(const QColor color);    

    bool eventFilter(QObject* obj, QEvent* event);        

    void dockPadding(const Qt::DockWidgetArea &area, QDockWidget *obj);

    void statusWidgets();

    void resizeEvent(QResizeEvent *event);

    void tabifiedDockWidgetActivated(QDockWidget *dockWidget);

    int signOf(const float x);

    void createDialog(const QString title);

    void zoomToMouse(const QWheelEvent *wheel, const double incrementValue);
};

#endif // MAINWINDOW_H
