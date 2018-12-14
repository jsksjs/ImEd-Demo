#include "layer.h"
#include "layerbutton.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glsheet.h"
#include "optiondialog.h"
#include "centralcontainer.h"

#include <QDockWidget>
#include <QListWidget>
#include <QToolButton>
#include <QtDebug>
#include <QMouseEvent>
#include <QEvent>
#include <QColorDialog>
#include <QLabel>
#include <QOpenGLWidget>
#include <QScrollBar>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);
    pColor = Qt::red;
    sColor = Qt::blue;
    primary = new QColorDialog(pColor);
    secondary = new QColorDialog(sColor);
    docks = QList<QDockWidget*>() << ui->toolDock << ui->optionDock << ui->colorDock << ui->layerDock;
    bars = QList<QWidget*>() << ui->statusBar << ui->menuBar << ui->mainToolBar;
    statusWidgets();
    layerBtns = new QButtonGroup();
    createCanvas();
    ui->paintBrushCombo->addItems(QStringList({"Square", "Circle"}));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toolDock_dockLocationChanged(const Qt::DockWidgetArea &area)
{
    dockPadding(area, ui->toolDock);
}

void MainWindow::on_optionDock_dockLocationChanged(const Qt::DockWidgetArea &area)
{
    dockPadding(area, ui->optionDock);
}

void MainWindow::on_colorDock_dockLocationChanged(const Qt::DockWidgetArea &area)
{
    dockPadding(area, ui->colorDock);
}

void MainWindow::on_layerDock_dockLocationChanged(const Qt::DockWidgetArea &area)
{
    dockPadding(area, ui->layerDock);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        const QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(obj == centralWidget() || obj == canvas) {
            if(mouseEvent->buttons() == Qt::MidButton) {
                if(last.isNull()) {
                    last = mouseEvent->globalPos();
                }
                QPoint now = mouseEvent->globalPos();
                QPoint delta = now - last;
                canvas->translate(canvas->pos() + delta);
                last = now;
            }
            else{
                last = QPoint();
            }
            if(obj == canvas) {
                const QList<int> sizes({ui->toolDock->width(), ui->optionDock->width(), ui->colorDock->width(), ui->layerDock->width()});
                QVector2D pos = canvas->screenCoordinates(mouseEvent->pos());
                pos.setX(qBound(1, int(pos.x()), canvas->getOriginalSize().width()));
                pos.setY(qBound(1, int(pos.y()), canvas->getOriginalSize().height()));
                mouseLocation->setText(QString(tr("Pos:(%1,%2)")).arg(QString::number(pos.x()).leftJustified(4, ' '),
                                                                      QString::number(pos.y()).rightJustified(4, ' ')));
                resizeDocks(docks, sizes, Qt::Horizontal);
            }
        }
        return false;
    }
    else if(event->type() == QEvent::Wheel) {
        const QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
        if((obj == centralWidget() || obj == canvas) && (wheel->buttons() != Qt::LeftButton && wheel->buttons() != Qt::RightButton)) {
            // Quick
            if(wheel->modifiers() == Qt::ControlModifier) {
                zoomToMouse(wheel, log2(canvas->getActualSize().width() * canvas->getActualSize().height()));
                return true;
            }
            // Medium
            else if(wheel->modifiers() == (Qt::ControlModifier | Qt::AltModifier)
                    || wheel->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
                zoomToMouse(wheel, log(canvas->getOriginalSize().width() * canvas->getOriginalSize().height()));
                return true;
            }
            // Fine
            else if(wheel->modifiers() == (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier)) {
                zoomToMouse(wheel, log10(cbrt(canvas->getOriginalSize().width() * canvas->getOriginalSize().height())));
                return true;
            }
        }
    }
    else
        return QObject::eventFilter(obj, event);
    return false;
}

void MainWindow::dockPadding(const Qt::DockWidgetArea &area, QDockWidget *obj)
{
    switch(area) {
    case Qt::RightDockWidgetArea:
        obj->setLayoutDirection(Qt::RightToLeft);
        obj->setProperty("right", true);
        obj->style()->unpolish(obj);
        obj->style()->polish(obj);
        break;
    case Qt::LeftDockWidgetArea:
        obj->setLayoutDirection(Qt::LeftToRight);
        obj->setProperty("right", false);
        obj->style()->unpolish(obj);
        obj->style()->polish(obj);
        break;
    default:
        break;
    }
}

void MainWindow::statusWidgets()
{
    frames = new QLabel();
    statusBar()->addPermanentWidget(frames);
    mouseLocation = new QLabel();
    statusBar()->addPermanentWidget(mouseLocation);

    zoomPercent = new QDoubleSpinBox(this);
    zoomPercent->setDecimals(2);
    zoomPercent->setMaximum(960000);
    zoomPercent->setMinimum(0.01);
    zoomPercent->setSuffix("%");
    zoomPercent->setValue(100);

    zoomPercent->setStyleSheet(QString("QDoubleSpinBox{ background-color: transparent; }"));

    statusBar()->addWidget(zoomPercent);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    const qint16 maxWidth = this->width()/3;
    foreach (QDockWidget *a, docks) {
        a->setMaximumWidth(maxWidth);
    }
}

void MainWindow::createCanvas(const QSize size, const QPoint pos)
{
    centralWidget()->layout()->removeWidget(canvas);
    canvas = new GLSheet(centralWidget(), size, colorToNVecF(pColor), colorToNVecF(sColor));
    canvas->translate(pos);
    centralWidget()->layout()->addWidget(canvas);

    foreach(QWidget *a, bars){
        a->raise();
    }
    foreach(QDockWidget *b, docks){
        b->raise();
    }

    disconnect(ui->centralWidget, &CentralContainer::movement, canvas, &GLSheet::parentMoved);
    connect(ui->centralWidget, &CentralContainer::movement, canvas, &GLSheet::parentMoved);

    disconnect(canvas, &GLSheet::scaleChanged, zoomPercent, &QDoubleSpinBox::setValue);
    connect(canvas, &GLSheet::scaleChanged, zoomPercent, &QDoubleSpinBox::setValue);

    connect(ui->paintSizeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](int s){
        canvas->brushSize(s);
    });

    disconnect(ui->checkBoxAA, &QCheckBox::toggled, canvas, &GLSheet::antialiased);
    connect(ui->checkBoxAA, &QCheckBox::toggled, canvas, &GLSheet::antialiased);

    connect(zoomPercent, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double d){
        canvas->scalePercent(d);
    });

    disconnect(canvas, &GLSheet::frameInfo, frames, &QLabel::setText);
    connect(canvas, &GLSheet::frameInfo, frames, &QLabel::setText);

    canvas->scalePercent(zoomPercent->value());
    canvas->antialiased(ui->checkBoxAA->isChecked());
    canvas->brushSize(ui->paintSizeSpin->value());
    canvas->setGridEnabled(ui->actionGrid->isChecked());

    // layer
    while (QLayoutItem* item = ui->layersLayout->takeAt(0) )
    {
        Q_ASSERT( ! item->layout() ); // otherwise the layout will leak
        delete item->widget();
        delete item;
    }
    numLayers = 0;
    QString title = QString("Layer %1").arg(numLayers);
    LayerButton *l = new LayerButton(nullptr, title);
    ui->layersLayout->insertWidget(0, l);
    delete layerBtns;
    layerBtns = new QButtonGroup();
    layerBtns->addButton(l);
    l->setChecked(true);
    lastChecked = l;
    numLayers++;

    disconnect(canvas, &GLSheet::preview, l, &LayerButton::preview);
    connect(canvas, &GLSheet::preview, l, &LayerButton::preview);

    connect(layerBtns, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonReleased),
            [=](QAbstractButton *button){ this->selectLayer(button); });
}

void MainWindow::grid(const bool e)
{
    canvas->setGridEnabled(e);
}

QVector4D MainWindow::colorToNVecF(const QColor color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

void MainWindow::reinitializeCanvas(const QSize size)
{
    QPoint curr = canvas->pos();
    delete canvas;
    createCanvas(size, curr);
}

int MainWindow::signOf(const float x)
{
    return (x > 0) - (x < 0);
}

void MainWindow::createDialog(const QString title)
{
    option = new OptionDialog(this, title, canvas->getOriginalSize());
    connect(option, &OptionDialog::sizeChanged, this, &MainWindow::reinitializeCanvas);
    option->show();
}

void MainWindow::zoomToMouse(const QWheelEvent *wheel, const double incrementValue)
{
    const double old = zoomPercent->value() / 100;
    const double curr = zoomPercent->value() + (signOf(wheel->delta()) * incrementValue);

    const QPointF deltaToPos = wheel->posF() / old - canvas->getPosDelta() / old;
    QPointF delta = deltaToPos * (curr / 100) - deltaToPos * old;
    delta.rx() = -delta.x();
    delta.ry() = -delta.y();
    zoomPercent->setValue(curr);
    canvas->translate(delta.toPoint());
}

void MainWindow::center()
{
    canvas->moveToCenter();
}

void MainWindow::left()
{
    canvas->moveToLeft();
}

void MainWindow::right()
{
    canvas->moveToRight();
}

void MainWindow::fit()
{
    canvas->fitScreen();
}

void MainWindow::original()
{
    canvas->scalePercent(100.0);
}

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    canvas->moveToCenter();
}

void MainWindow::on_colPrimary_released()
{
    pColor = primary->getColor(pColor, this, tr("Choose Primary"), QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
    if(pColor.isValid()) {
        QPushButton *btn = ui->colPrimary;
        btn->setStyleSheet(btn->styleSheet().replace(QRegularExpression("QPushButton{\n\tbackground-color:.+;"), QString("QPushButton{\n\tbackground-color: rgba(%1, %2, %3, %4);").arg(pColor.red()).arg(pColor.green()).arg(pColor.blue())).arg(pColor.alpha()));
        canvas->drawColor(colorToNVecF(pColor));
    }
}

void MainWindow::on_colSecondary_released()
{
    sColor = secondary->getColor(sColor, this, tr("Choose Secondary"), QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
    if(sColor.isValid()) {
        QPushButton *btn = ui->colSecondary;
        btn->setStyleSheet(btn->styleSheet().replace(QRegularExpression("QPushButton{\n\tbackground-color:.+;"), QString("QPushButton{\n\tbackground-color: rgba(%1, %2, %3, %4);").arg(sColor.red()).arg(sColor.green()).arg(sColor.blue())).arg(sColor.alpha()));
        canvas->secondaryColor(colorToNVecF(sColor));
    }
}

void MainWindow::on_colorDock_topLevelChanged(bool topLevel)
{
    QWidget *contents = ui->colDockContents;
    if(topLevel) {
        contents->layout()->setMargin(2);
    }
    else{
        contents->layout()->setMargin(0);
    }
}

void MainWindow::dialogNew()
{
    createDialog("New");
}

void MainWindow::on_New_released()
{
    QString title = QString("Layer %1").arg(numLayers);
    LayerButton *l = new LayerButton(nullptr, title);
    ui->layersLayout->insertWidget(ui->layersLayout->indexOf(layerBtns->checkedButton()), l);
    layerBtns->addButton(l);
    canvas->insertLayer();
    numLayers++;
}

void MainWindow::selectLayer(QAbstractButton *l)
{
    disconnect(canvas, &GLSheet::preview, lastChecked, &LayerButton::preview);
    connect(canvas, &GLSheet::preview, static_cast<LayerButton*>(l), &LayerButton::preview);
    canvas->setCurrentLayer(ui->layersLayout->indexOf(l));
    lastChecked = static_cast<LayerButton*>(l);
}

void MainWindow::on_Del_released()
{
    if(layerBtns->buttons().size() > 1){
        const int pos = ui->layersLayout->indexOf(lastChecked);
        qDebug() << pos << ui->layers->children().size();
        // -1 for layout, -1 for size, so -2
        if(pos < ui->layers->children().size()-2){
            layerBtns->checkedButton()->setChecked(false);
            static_cast<LayerButton*>(ui->layersLayout->itemAt(pos+1)->widget())->setChecked(true);
        }
        else{
            layerBtns->checkedButton()->setChecked(false);
            static_cast<LayerButton*>(ui->layersLayout->itemAt(pos-1)->widget())->setChecked(true);
        }
        selectLayer(layerBtns->checkedButton());
        QLayoutItem *item = ui->layersLayout->takeAt(pos);
        delete item->widget();
        delete item;
        canvas->deleteLayer(pos+1);
        update();
    }
}
