#ifndef GLSHEET_H
#define GLSHEET_H

#include "camera.h"
#include "transform.h"
#include "vertex2D.h"
#include "paint.h"
#include "tool.h"
#include "layer.h"

#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <chrono>
#include <vector>

class GLSheet : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GLSheet(QWidget *parent = nullptr, const QSize wh = QSize(256, 256),
                     const QVector4D primary = QVector4D(1.0f, 0.0f, 0.0f, 1.0f),
                     const QVector4D secondary = QVector4D(0.0f, 0.0f, 0.0f, 1.0f));

    ~GLSheet();

    QVector4D secondaryRGBA;
    QVector4D drawRGBA;

    void secondaryColor(const QVector4D secondary);
    void drawColor(const QVector4D primary);

    QSize getOriginalSize() const;
    QSize getActualSize() const;
    QVector2D glCoordinates(const QPoint position, const bool integers = true);
    QVector2D screenCoordinates(const QPoint position);

    QPoint getPosDelta();
    void fitScreen();
    void moveToCenter();
    void moveToLeft();
    void moveToRight();
    void translate(const QPoint &p);
    void setPos(const QPoint &p);

    void setGridEnabled(const bool e);

    void insertLayer();
    void duplicateLayer();
    void deleteLayer(const int pos);
    void swapLayer(const int pos);
    void transChngBrushShader(const QString &option);
signals:
    void scaleChanged(const double s);    
    void frameInfo(const QString info);
    void preview(const QImage i);
public slots:
    void scale(const double s);
    void scalePercent(const double s);
    void brushSize(const int s);
    void antialiased(const bool aa);
    void parentMoved(const QPoint p);
    void setCurrentLayer(const int pos);    
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(const int w, const int h);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:        
    QOpenGLFunctions f;
    QOpenGLBuffer m_vertex, m_index;
    QOpenGLVertexArrayObject m_object;    
    std::vector<QOpenGLShaderProgram*> m_program;
    QOpenGLFramebufferObject *m_fbo;

    QOpenGLTexture *background;

    enum Shaders{SQUARE, CIRCLE, TEXTURE};
    Shaders currentBrushShader = SQUARE;
    void changeBrushShader(Shaders &s);

    QSize originalSize, actualSize;

    std::list<Layer> layers;
    std::list<Layer>::iterator currentLayer;
    Tool *currentTool = new Paint();

    std::vector<Vertex2D> layerTarget, backTarget;
    std::vector<unsigned int> layerTargetIndex;

    std::vector<Vertex2D> currentMesh;
    std::vector<unsigned int> currentIndex;
    std::map<const Vertex2D, unsigned int> currentMeshInd;

    std::vector<Vertex2D> grid;
    std::vector<unsigned int> gridIndex;
    std::map<const Vertex2D, unsigned int> gridMeshInd;

    Vertex2D lastMouseVertex;
    bool draw = true;
    void insertMesh(std::vector<Vertex2D> &mesh, std::vector<unsigned int> &index, std::map<const Vertex2D, unsigned int> &meshInd, const std::vector<Vertex2D> &v);
    void drawBrush(const QVector2D point, const QVector4D color);
    int m_brushSize = 0;
    bool m_aa = false;    

    Transform trans;
    Camera cam;

    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();
    int frames = 0;
    void frameTime();

    void BresenhamLine(const QVector2D &first, const QVector2D &second, const QVector4D &color);
    void BresenhamLineLow(const QVector2D &first, const QVector2D &second, const QVector4D &color);
    void BresenhamLineHigh(const QVector2D &first, const QVector2D &second, const QVector4D &color);

    void WuLine(QVector2D first, QVector2D second, const QVector4D color);
    float fpart(const float x);
    float rfpart(const float x);

    bool rendered = false;

    void fillGrid(const float width, const QVector4D color);
    bool drawGrid = false;
};

#endif // GLSHEET_H
