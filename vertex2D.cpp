#include "vertex2D.h"


Q_DECLARE_TYPEINFO(Vertex2D, Q_MOVABLE_TYPE);

Vertex2D::Vertex2D(const QVector2D &position) : m_position(position) {
}

Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color) : m_position(position), m_color(color){
}

Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color, const QVector3D &offset)
    : m_position(position), m_color(color), m_offset(offset) {
}

Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color, const QVector2D &offset)
    : m_position(position), m_color(color), m_offset(QVector3D(offset, 0)){
}

Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color, const QVector3D &offset, const QVector2D &texCoords)
    : m_position(position), m_color(color), m_offset(offset), m_texCoords(texCoords){
}

Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color, const QVector2D &offset, const QVector2D &texCoords)
    : m_position(position), m_color(color), m_offset(QVector3D(offset, 0)), m_texCoords(texCoords){
}

Vertex2D::Vertex2D(const Vertex2D &other)
    : m_position(other.position()), m_color(other.color()), m_offset(other.offset()), m_texCoords(other.texCoords()){
}

const QVector2D& Vertex2D::position() const {
    return m_position;
}

const QVector4D& Vertex2D::color() const {
    return m_color;
}

const QVector3D &Vertex2D::offset() const {
    return m_offset;
}

const QVector2D &Vertex2D::texCoords() const {
    return m_texCoords;
}

void Vertex2D::setPosition(const QVector2D &position){
    m_position = position;
}

void Vertex2D::setColor(const QVector4D &color){
    m_color = color;
}

void Vertex2D::setOffset(const QVector3D &offset){
    m_offset = offset;
}

void Vertex2D::setTexCoords(const QVector2D &tex){
    m_texCoords = tex;
}

int Vertex2D::positionOffset(){
    return offsetof(Vertex2D, m_position);
}

int Vertex2D::colorOffset(){
    return offsetof(Vertex2D, m_color);
}

int Vertex2D::offsetOffset(){
    return offsetof(Vertex2D, m_offset);
}

int Vertex2D::texCoordsOffset(){
    return offsetof(Vertex2D, m_texCoords);
}

int Vertex2D::stride(){
    return sizeof(Vertex2D);
}
