#include "centralcontainer.h"

#include <QDebug>
#include <QMoveEvent>

CentralContainer::CentralContainer(QWidget *parent) : QWidget(parent){ }

/**
 * @brief CentralContainer::moveEvent
 * @param event
 */
void CentralContainer::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    if(event->oldPos().x() != event->pos().x()) {
        emit movement(QPoint(event->oldPos().x() - event->pos().x(), 0));
    }
}
