#ifndef LAYERBUTTON_H
#define LAYERBUTTON_H

#include "layer.h"

#include <QMouseEvent>
#include <QPushButton>

class LayerButton : public QPushButton
{
    Q_OBJECT
public:
    LayerButton(QWidget *parent = nullptr, const QString name = "Layer 000");
    void setPreview(const QImage i);
    QImage const& getPreview() const;
public slots:
    void preview(const QImage i);    
protected:
    void paintEvent(QPaintEvent *);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseClickEvent(QMouseEvent *event);
private:
    QImage m_preview;
    qreal m_aspect, m_scaleWidth, m_scaleHeight;
};

#endif // LAYERBUTTON_H
