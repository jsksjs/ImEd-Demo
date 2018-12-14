#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QVector3D>
#include <QMatrix4x4>


class Transform
{
public:
    Transform(const QVector3D &pos = QVector3D(), const QVector3D &rot = QVector3D(), const QVector3D &scale = QVector3D(1.0f, 1.0f, 1.0f));

    QMatrix4x4 GetModel() const;

    QVector3D& GetPos();
    QVector3D& GetRot();
    QVector3D& GetScale();

    void SetPos(const QVector3D &pos);
    void SetRot(const QVector3D &rot);
    void SetScale(const QVector3D &scale);

private:
    QVector3D m_pos;
    QVector3D m_rot;
    QVector3D m_scale;
};

#endif // TRANSFORM_H
