#ifndef LAYER_H
#define LAYER_H

#include "tool.h"
#include "vertex2D.h"

#include <vector>
#include <QSize>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QImage>

class Layer
{
public:
    Layer(const QSize &size);
    Layer(const Layer &other, QOpenGLFunctions *f);

    void DrawBuffer(QOpenGLFunctions &f, QOpenGLBuffer *vertex, QOpenGLBuffer *index,
              std::vector<Vertex2D> *currentMesh, std::vector<unsigned int> *currentIndex);
    void DrawTexture(QOpenGLBuffer *vertex);
    void Bind();
    void Release();
    QImage Image();
private:
    QSize m_size;
    QOpenGLFramebufferObject *m_fbo;

    std::vector<Vertex2D> layerTarget;
    static std::vector<unsigned int> layerTargetIndex;    
};

#endif // LAYER_H
