#ifndef TOOL_H
#define TOOL_H

#include <QMouseEvent>



class Tool
{
public:
    virtual ~Tool() {}
    virtual void ready() = 0;
    virtual void mouseMoveEvent(QMouseEvent *event) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event) = 0;
    virtual void mousePressEvent(QMouseEvent *event) = 0;
    virtual void bindCurrentShader() = 0;
    virtual void cleanUp() = 0;
};

#endif // TOOL_H
