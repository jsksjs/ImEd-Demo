#include "transform.h"
#include <QMatrix4x4>
#include <QVector3D>

/**
 * @brief Transform::Transform
 * @param pos
 * @param rot
 * @param scale
 */
Transform::Transform(const QVector3D &pos, const QVector3D &rot, const QVector3D &scale) :
    m_pos(pos),
    m_rot(rot),
    m_scale(scale){
}

/**
 * @brief Transform::GetModel
 * @return
 */
QMatrix4x4 Transform::GetModel() const
{
    QMatrix4x4 posMatrix = QMatrix4x4();
    posMatrix.translate(m_pos);

    QMatrix4x4 rotXMatrix = QMatrix4x4();
    rotXMatrix.rotate(m_rot.x(), QVector3D(1,0,0));

    QMatrix4x4 rotYMatrix = QMatrix4x4();
    rotYMatrix.rotate(m_rot.y(), QVector3D(0,1,0));

    QMatrix4x4 rotZMatrix = QMatrix4x4();
    rotZMatrix.rotate(m_rot.z(), QVector3D(0,0,1));

    QMatrix4x4 scaleMatrix = QMatrix4x4();
    scaleMatrix.scale(m_scale);

    QMatrix4x4 rotMatrix = rotZMatrix * rotYMatrix * rotXMatrix;

    return posMatrix * rotMatrix * scaleMatrix;
}

/**
 * @brief Transform::GetPos
 * @return
 */
QVector3D &Transform::GetPos()
{
    return m_pos;
}

/**
 * @brief Transform::GetRot
 * @return
 */
QVector3D &Transform::GetRot()
{
    return m_rot;
}

/**
 * @brief Transform::GetScale
 * @return
 */
QVector3D &Transform::GetScale()
{
    return m_scale;
}

/**
 * @brief Transform::SetPos
 * @param pos
 */
void Transform::SetPos(const QVector3D &pos)
{
    m_pos = pos;
}

/**
 * @brief Transform::SetRot
 * @param rot
 */
void Transform::SetRot(const QVector3D &rot)
{
    m_rot = rot;
}

/**
 * @brief Transform::SetScale
 * @param scale
 */
void Transform::SetScale(const QVector3D &scale)
{
    m_scale = scale;
}


