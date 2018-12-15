#include "camera.h"

/**
 * @brief Camera::Camera
 * @param pos
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param nearPlane
 * @param farPlane
 */
Camera::Camera(const QVector3D &pos, const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane)
{
    m_ortho.ortho(left, right, bottom, top, nearPlane, farPlane);
    m_position = pos;
    m_forward = QVector3D(0, 0, -1);
    m_up = QVector3D(0, 1, 0);
}

/**
 * @brief Camera::GetViewProjection
 * @return
 */
QMatrix4x4 Camera::GetViewProjection() const
{
    QMatrix4x4 view;
    view.lookAt(m_position, m_position + m_forward, m_up);
    return m_ortho * view;
}
