#include "transform.h"
#include <QMatrix4x4>
#include <QVector3D>

Transform::Transform(const QVector3D &pos, const QVector3D &rot, const QVector3D &scale) :
    m_pos(pos),
    m_rot(rot),
    m_scale(scale){
}

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

QVector3D &Transform::GetPos()
{
    return m_pos;
}

QVector3D &Transform::GetRot()
{
    return m_rot;
}

QVector3D &Transform::GetScale()
{
    return m_scale;
}

void Transform::SetPos(const QVector3D &pos)
{
    m_pos = pos;
}

void Transform::SetRot(const QVector3D &rot)
{
    m_rot = rot;
}

void Transform::SetScale(const QVector3D &scale)
{
    m_scale = scale;
}


