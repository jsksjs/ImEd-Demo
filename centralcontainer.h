#ifndef CENTRALCONTAINER_H
#define CENTRALCONTAINER_H

#include <QWidget>

class CentralContainer : public QWidget
{
    Q_OBJECT
public:
    CentralContainer(QWidget *parent);

signals:
    void movement(const QPoint p);

protected:
    void moveEvent(QMoveEvent *event);
};

#endif // CENTRALCONTAINER_H
