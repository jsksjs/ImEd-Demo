#include "layerbutton.h"

#include <QStyleOptionToolButton>
#include <QStylePainter>
#include <QDebug>

/**
 * @brief LayerButton::LayerButton
 * @param parent
 * @param name
 */
LayerButton::LayerButton(QWidget *parent, QString name) : QPushButton(parent)
{
    this->setMinimumSize(QSize(50, 80));
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    this->setCheckable(true);
    this->setAutoExclusive(true);
    this->setText(name);    
}

/**
 * @brief LayerButton::setPreview
 * @param i
 */
void LayerButton::setPreview(const QImage i)
{
    m_preview = i;
    m_aspect = std::min(75.0 / i.width(), 75.0 / i.height());
    m_scaleWidth = i.width() * m_aspect;
    m_scaleHeight = i.height() * m_aspect;
    update();
}

/**
 * @brief LayerButton::getPreview
 * @return
 */
const QImage &LayerButton::getPreview() const
{
    return m_preview;
}

/**
 * @brief LayerButton::preview
 * @param i
 */
void LayerButton::preview(const QImage i)
{
    setPreview(i);
}

/**
 * @brief LayerButton::paintEvent
 */
void LayerButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionButton opt;
    initStyleOption(&opt);
    p.drawControl(QStyle::CE_PushButton, opt);
    QPainter a(this);    
    a.drawImage(QRectF(5, 5, m_scaleWidth, m_scaleHeight), m_preview, QRectF(0, 0, m_preview.width(), m_preview.height()));
}

/**
 * @brief LayerButton::mouseDoubleClickEvent
 * @param event
 */
void LayerButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    //dialogue, option menu for layer, can change name
}
