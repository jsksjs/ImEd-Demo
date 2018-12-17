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

#include <QDebug>

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
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

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_toolDock_dockLocationChanged
 * @param area
 */
void MainWindow::on_toolDock_dockLocationChanged(const Qt::DockWidgetArea &area)
{
    dockPadding(area, ui->toolDock);
}

/**
 * @brief MainWindow::on_optionDock_dockLocationChanged
 * @param area
 */
void MainWindow::on_optionDock_dockLocationChanged(const Qt::DockWidgetArea &area)
{
    dockPadding(area, ui->optionDock);
}

/**
 * @brief MainWindow::on_colorDock_dockLocationChanged
 * @param area
 */
void MainWindow::on_colorDock_dockLocationChanged(const Qt::DockWidgetArea &area)
{
    dockPadding(area, ui->colorDock);
}

/**
 * @brief MainWindow::on_layerDock_dockLocationChanged
 * @param area
 */
void MainWindow::on_layerDock_dockLocationChanged(const Qt::DockWidgetArea &area)
{
    dockPadding(area, ui->layerDock);
}

/**
 * @brief MainWindow::eventFilter
 * @param obj
 * @param event
 * @return
 */
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
                mouseLocation->setText(QString(tr("Pos:(%1,%2)")).arg(QString::number(static_cast<double>(pos.x())).leftJustified(4, ' '),
                                                                      QString::number(static_cast<double>(pos.y())).rightJustified(4, ' ')));
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

/**
 * @brief MainWindow::dockPadding
 * @param area
 * @param obj
 */
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

/**
 * @brief MainWindow::statusWidgets
 */
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

/**
 * @brief MainWindow::resizeEvent
 * @param event
 */
void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    const qint16 maxWidth = static_cast<qint16>(this->width()/3);
    foreach (QDockWidget *a, docks) {
        a->setMaximumWidth(maxWidth);
    }
}

/**
 * @brief MainWindow::createCanvas
 * @param size
 * @param pos
 */
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
    canvas->brushSize(static_cast<int>(ui->paintSizeSpin->value()));
    canvas->setGridEnabled(ui->actionGrid->isChecked());

    // layer
    while (QLayoutItem* item = ui->layersLayout->takeAt(0) )
    {
        Q_ASSERT(!item->layout()); // otherwise the layout will leak
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

/**
 * @brief MainWindow::grid
 * @param e
 */
void MainWindow::grid(const bool e)
{
    canvas->setGridEnabled(e);
}

/**
 * @brief MainWindow::colorToNVecF
 * @param color
 * @return
 */
QVector4D MainWindow::colorToNVecF(const QColor color)
{
    return QVector4D(static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF()), static_cast<float>(color.alphaF()));
}

/**
 * @brief MainWindow::reinitializeCanvas
 * @param size
 */
void MainWindow::reinitializeCanvas(const QSize size)
{
    QPoint curr = canvas->pos();
    delete canvas;
    createCanvas(size, curr);
}

/**
 * @brief MainWindow::signOf
 * @param x
 * @return
 */
int MainWindow::signOf(const float x)
{
    return (x > 0) - (x < 0);
}

/**
 * @brief MainWindow::createDialog
 * @param title
 */
void MainWindow::createDialog(const QString title)
{
    option = new OptionDialog(this, title, canvas->getOriginalSize());
    connect(option, &OptionDialog::sizeChanged, this, &MainWindow::reinitializeCanvas);
    option->show();
}

/**
 * @brief MainWindow::zoomToMouse
 * @param wheel
 * @param incrementValue
 */
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

/**
 * @brief MainWindow::center
 */
void MainWindow::center()
{
    canvas->moveToCenter();
}

/**
 * @brief MainWindow::left
 */
void MainWindow::left()
{
    canvas->moveToLeft();
}

/**
 * @brief MainWindow::right
 */
void MainWindow::right()
{
    canvas->moveToRight();
}

/**
 * @brief MainWindow::fit
 */
void MainWindow::fit()
{
    canvas->fitScreen();
}

/**
 * @brief MainWindow::original
 */
void MainWindow::original()
{
    canvas->scalePercent(100.0);
}

/**
 * @brief MainWindow::showEvent
 * @param event
 */
void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    canvas->moveToCenter();
}

/**
 * @brief MainWindow::on_colPrimary_released
 */
void MainWindow::on_colPrimary_released()
{
    pColor = primary->getColor(pColor, this, tr("Choose Primary"), QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
    if(pColor.isValid()) {
        QPushButton *btn = ui->colPrimary;
        btn->setStyleSheet(btn->styleSheet().replace(QRegularExpression("QPushButton{\n\tbackground-color:.+;"), QString("QPushButton{\n\tbackground-color: rgba(%1, %2, %3, %4);").arg(pColor.red()).arg(pColor.green()).arg(pColor.blue())).arg(pColor.alpha()));
        canvas->drawColor(colorToNVecF(pColor));
    }
}

/**
 * @brief MainWindow::on_colSecondary_released
 */
void MainWindow::on_colSecondary_released()
{
    sColor = secondary->getColor(sColor, this, tr("Choose Secondary"), QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
    if(sColor.isValid()) {
        QPushButton *btn = ui->colSecondary;
        btn->setStyleSheet(btn->styleSheet().replace(QRegularExpression("QPushButton{\n\tbackground-color:.+;"), QString("QPushButton{\n\tbackground-color: rgba(%1, %2, %3, %4);").arg(sColor.red()).arg(sColor.green()).arg(sColor.blue())).arg(sColor.alpha()));
        canvas->secondaryColor(colorToNVecF(sColor));
    }
}

/**
 * @brief MainWindow::on_colorDock_topLevelChanged
 * @param topLevel
 */
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

/**
 * @brief MainWindow::dialogNew
 */
void MainWindow::dialogNew()
{
    createDialog("New");
}

/**
 * @brief MainWindow::on_New_released
 */
void MainWindow::on_New_released()
{
    QString title = QString("Layer %1").arg(numLayers);
    LayerButton *l = new LayerButton(nullptr, title);
    ui->layersLayout->insertWidget(ui->layersLayout->indexOf(layerBtns->checkedButton()), l);
    layerBtns->addButton(l);
    canvas->insertLayer();
    numLayers++;
}

/**
 * @brief MainWindow::selectLayer
 * @param l
 */
void MainWindow::selectLayer(QAbstractButton *l)
{
    disconnect(canvas, &GLSheet::preview, lastChecked, &LayerButton::preview);
    connect(canvas, &GLSheet::preview, static_cast<LayerButton*>(l), &LayerButton::preview);
    canvas->setCurrentLayer(ui->layersLayout->indexOf(l));
    l->setChecked(true);
    lastChecked = static_cast<LayerButton*>(l);
}

/**
 * @brief MainWindow::on_Del_released
 */
void MainWindow::on_Del_released()
{
    if(layerBtns->buttons().size() > 1){
        const int pos = ui->layersLayout->indexOf(lastChecked);
        layerBtns->checkedButton()->setChecked(false);
        // -1 for layout, -1 for size, so -2
        if(pos < ui->layers->children().size()-2){
            static_cast<LayerButton*>(ui->layersLayout->itemAt(pos+1)->widget())->setChecked(true);
        }
        else{
            static_cast<LayerButton*>(ui->layersLayout->itemAt(pos-1)->widget())->setChecked(true);
        }
        selectLayer(layerBtns->checkedButton());
        QLayoutItem *item = ui->layersLayout->takeAt(pos);
        delete item->widget();
        canvas->deleteLayer(pos+1);
        update();
    }
}

/**
 * @brief MainWindow::on_Dup_released
 */
void MainWindow::on_Dup_released()
{
    QString title = layerBtns->checkedButton()->text();
    LayerButton *l = new LayerButton(nullptr, title);
    ui->layersLayout->insertWidget(ui->layersLayout->indexOf(layerBtns->checkedButton()), l);
    layerBtns->addButton(l);
    canvas->duplicateLayer();
    l->preview(static_cast<LayerButton*>(layerBtns->checkedButton())->getPreview());
}

/**
 * @brief MainWindow::on_Up_released
 */
void MainWindow::on_Up_released()
{
    if(layerBtns->buttons().size() > 1){
        const int pos = ui->layersLayout->indexOf(lastChecked);
        if(pos > 0){
            QLayoutItem *item = ui->layersLayout->takeAt(pos);
            ui->layersLayout->insertWidget(pos-1, item->widget());
            canvas->swapLayer(pos-1);
            static_cast<LayerButton*>(ui->layersLayout->itemAt(pos-1)->widget())->setChecked(true);
            selectLayer(static_cast<QAbstractButton*>(item->widget()));
            update();
        }
    }
}

/**
 * @brief MainWindow::on_Dw_released
 */
void MainWindow::on_Dw_released()
{
    if(layerBtns->buttons().size() > 1){
        const int pos = ui->layersLayout->indexOf(lastChecked);
        if(pos < layerBtns->buttons().size()-1){
            QLayoutItem *item = ui->layersLayout->takeAt(pos);
            ui->layersLayout->insertWidget(pos+1, item->widget());
            canvas->swapLayer(pos+1);
            selectLayer(static_cast<QAbstractButton*>(item->widget()));
            update();
        }
    }
}

/**
 * @brief MainWindow::on_paintBrushCombo_activated
 * @param option
 */
void MainWindow::on_paintBrushCombo_activated(const QString &option)
{
    canvas->transChngBrushShader(option);
}
