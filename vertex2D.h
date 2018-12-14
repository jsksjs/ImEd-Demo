#ifndef Vertex2D_H
#define Vertex2D_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

class Vertex2D
{
public:
    explicit Vertex2D(const QVector2D &position);
    Vertex2D(const QVector2D &position, const QVector4D &color);
    Vertex2D(const QVector2D &position, const QVector4D &color, const QVector3D &offset);
    Vertex2D(const QVector2D &position, const QVector4D &color, const QVector2D &offset);
    Vertex2D(const QVector2D &position, const QVector4D &color, const QVector3D &offset, const QVector2D &texCoords);
    Vertex2D(const QVector2D &position, const QVector4D &color, const QVector2D &offset, const QVector2D &texCoords);
    Vertex2D(const Vertex2D &other);
    Vertex2D() {}

    const QVector2D& position() const;
    const QVector4D& color() const;
    const QVector3D& offset() const;
    const QVector2D& texCoords() const;
    void setPosition(const QVector2D &position);
    void setColor(const QVector4D &color);
    void setOffset(const QVector3D &offset);
    void setTexCoords(const QVector2D &tex);

    static const int PositionTupleSize = 2;
    static const int ColorTupleSize = 4;
    static const int OffsetTupleSize = 3;
    static const int TexCoordsTupleSize = 2;
    static int positionOffset();
    static int colorOffset();
    static int offsetOffset();
    static int texCoordsOffset();
    static int stride();

    inline friend bool operator==(const Vertex2D &first, const Vertex2D &other) {
        return (first.position().x() == other.position().x()
                && first.position().y() == other.position().y()
                && first.color().x() == other.color().x()
                && first.color().y() == other.color().y()
                && first.color().z() == other.color().z()
                && first.color().w() == other.color().w()
                && first.offset().x() == other.offset().x()
                && first.offset().y() == other.offset().y()
                && first.offset().z() == other.offset().z());
    }

    inline friend bool operator!=(const Vertex2D &first, const Vertex2D &other) { return !(first==other); }

    // fix
    inline friend bool operator<(const Vertex2D &first, const Vertex2D &other) {
        if(first.position().x() != other.position().x())
            return first.position().x() < other.position().x();
        else if(first.position().y() != other.position().y())
            return first.position().y() < other.position().y();
        else if(first.color().x() != other.color().x())
            return first.color().x() < other.color().x();
        else if(first.color().y() != other.color().y())
            return first.color().y() < other.color().y();
        else if(first.color().z() != other.color().z())
            return first.color().z() < other.color().z();
        else if(first.color().w() != other.color().w())
            return first.color().w() < other.color().w();
        else if(first.offset().x() != other.offset().x())
            return first.offset().x() < other.offset().x();
        else if(first.offset().y() != other.offset().y())
            return first.offset().y() < other.offset().y();
        else if(first.offset().z() != other.offset().z())
            return first.offset().z() < other.offset().z();
        else
            return false;
    }

private:
    QVector2D m_position;
    QVector4D m_color;
    QVector3D m_offset;
    QVector2D m_texCoords;
};

#endif // Vertex2D_H
