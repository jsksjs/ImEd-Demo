#include "layer.h"

std::vector<unsigned int> Layer::layerTargetIndex = {0, 2, 1, 3, 1, 2};

Layer::Layer(const QSize &size)
{    
    m_fbo = new QOpenGLFramebufferObject(size, QOpenGLFramebufferObject::NoAttachment, GL_TEXTURE_2D, GL_RGBA8);
    Bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Release();

    const QVector4D temp(1.0f, 1.0f, 1.0f, 0.0f);
    layerTarget = {Vertex2D(QVector2D(0, 0), temp, QVector2D(0, 0), QVector2D(0.0f, 1.0f)),
               Vertex2D(QVector2D(0, size.height()), temp, QVector2D(0, 0), QVector2D(0.0f, 0.0f)),
               Vertex2D(QVector2D(size.width(), 0), temp, QVector2D(0, 0), QVector2D(1.0f, 1.0f)),
               Vertex2D(QVector2D(size.width(), size.height()), temp, QVector2D(0, 0), QVector2D(1.0f, 0.0f))};
    QColor back(55, 55, 55);
    glClearColor(back.redF(),back.greenF(),back.blueF(), 1.0f);
}

void Layer::DrawBuffer(QOpenGLFunctions &f, QOpenGLBuffer *vertex, QOpenGLBuffer *index,
                       std::vector<Vertex2D> *currentMesh, std::vector<unsigned int> *currentIndex)
{
    Bind();
    vertex->allocate(currentMesh->data(), sizeof(currentMesh->data()[0]) * currentMesh->size());
    index->allocate(currentIndex->data(), sizeof(currentIndex->data()[0]) * currentIndex->size());
    glDrawElements(GL_TRIANGLES, currentIndex->size(), GL_UNSIGNED_INT, nullptr);
    Release();    
}

void Layer::DrawTexture(QOpenGLBuffer *vertex)
{
    glBindTexture(GL_TEXTURE_2D, m_fbo->texture());
    vertex->allocate(layerTarget.data(), sizeof(layerTarget.data()[0]) * layerTarget.size());
    glDrawElements(GL_TRIANGLES, layerTargetIndex.size(), GL_UNSIGNED_INT, nullptr);
}

void Layer::Bind()
{
    m_fbo->bind();
    glViewport(0,0, m_size.width(), m_size.height());
}

void Layer::Release()
{
    m_fbo->release();
}

QImage Layer::Image()
{
    return m_fbo->toImage();
}
