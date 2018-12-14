#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
public:    
    Camera(const QVector3D &pos, const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane);

    QMatrix4x4 GetViewProjection() const;

private:
    QMatrix4x4 m_ortho;

    QVector3D m_position;
    QVector3D m_forward;
    QVector3D m_up;
};

#endif // CAMERA_H
