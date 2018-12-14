#ifndef PAINT_H
#define PAINT_H

#include "vertex2D.h"
#include "tool.h"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>


class Paint : public Tool
{
public:
    Paint();
    ~Paint();
protected:
    void ready();
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void bindCurrentShader();
    void cleanUp();
private:
    std::vector<QOpenGLShaderProgram*> m_shaders;
    QOpenGLShaderProgram *m_currentShader;

    Vertex2D lastMouseVertex;
    bool draw = true;
    void drawBrush(const QVector2D point, const QVector4D color);
    int m_brushSize = 0;
    bool m_aa = false;

    void BresenhamLine(const QVector2D &first, const QVector2D &second, const QVector4D &color);
    void BresenhamLineLow(const QVector2D &first, const QVector2D &second, const QVector4D &color);
    void BresenhamLineHigh(const QVector2D &first, const QVector2D &second, const QVector4D &color);

    void WuLine(QVector2D first, QVector2D second, const QVector4D color);
    float fpart(const float x);
    float rfpart(const float x);
};

#endif // PAINT_H
