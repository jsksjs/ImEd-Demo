#include "vertex2D.h"


Q_DECLARE_TYPEINFO(Vertex2D, Q_MOVABLE_TYPE);

/**
 * @brief Vertex2D::Vertex2D
 * @param position
 */
Vertex2D::Vertex2D(const QVector2D &position) : m_position(position) {
}

/**
 * @brief Vertex2D::Vertex2D
 * @param position
 * @param color
 */
Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color) : m_position(position), m_color(color){
}

/**
 * @brief Vertex2D::Vertex2D
 * @param position
 * @param color
 * @param offset
 */
Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color, const QVector3D &offset)
    : m_position(position), m_color(color), m_offset(offset) {
}

/**
 * @brief Vertex2D::Vertex2D
 * @param position
 * @param color
 * @param offset
 */
Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color, const QVector2D &offset)
    : m_position(position), m_color(color), m_offset(QVector3D(offset, 0)){
}

/**
 * @brief Vertex2D::Vertex2D
 * @param position
 * @param color
 * @param offset
 * @param texCoords
 */
Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color, const QVector3D &offset, const QVector2D &texCoords)
    : m_position(position), m_color(color), m_offset(offset), m_texCoords(texCoords){
}

/**
 * @brief Vertex2D::Vertex2D
 * @param position
 * @param color
 * @param offset
 * @param texCoords
 */
Vertex2D::Vertex2D(const QVector2D &position, const QVector4D &color, const QVector2D &offset, const QVector2D &texCoords)
    : m_position(position), m_color(color), m_offset(QVector3D(offset, 0)), m_texCoords(texCoords){
}

/**
 * @brief Vertex2D::Vertex2D
 * @param other
 */
Vertex2D::Vertex2D(const Vertex2D &other)
    : m_position(other.position()), m_color(other.color()), m_offset(other.offset()), m_texCoords(other.texCoords()){
}

/**
 * @brief Vertex2D::position
 * @return
 */
const QVector2D& Vertex2D::position() const {
    return m_position;
}

/**
 * @brief Vertex2D::color
 * @return
 */
const QVector4D& Vertex2D::color() const {
    return m_color;
}

/**
 * @brief Vertex2D::offset
 * @return
 */
const QVector3D &Vertex2D::offset() const {
    return m_offset;
}

/**
 * @brief Vertex2D::texCoords
 * @return
 */
const QVector2D &Vertex2D::texCoords() const {
    return m_texCoords;
}

/**
 * @brief Vertex2D::setPosition
 * @param position
 */
void Vertex2D::setPosition(const QVector2D &position){
    m_position = position;
}

/**
 * @brief Vertex2D::setColor
 * @param color
 */
void Vertex2D::setColor(const QVector4D &color){
    m_color = color;
}

/**
 * @brief Vertex2D::setOffset
 * @param offset
 */
void Vertex2D::setOffset(const QVector3D &offset){
    m_offset = offset;
}

/**
 * @brief Vertex2D::setTexCoords
 * @param tex
 */
void Vertex2D::setTexCoords(const QVector2D &tex){
    m_texCoords = tex;
}

/**
 * @brief Vertex2D::positionOffset
 * @return
 */
int Vertex2D::positionOffset(){
    return offsetof(Vertex2D, m_position);
}

/**
 * @brief Vertex2D::colorOffset
 * @return
 */
int Vertex2D::colorOffset(){
    return offsetof(Vertex2D, m_color);
}

/**
 * @brief Vertex2D::offsetOffset
 * @return
 */
int Vertex2D::offsetOffset(){
    return offsetof(Vertex2D, m_offset);
}

/**
 * @brief Vertex2D::texCoordsOffset
 * @return
 */
int Vertex2D::texCoordsOffset(){
    return offsetof(Vertex2D, m_texCoords);
}

/**
 * @brief Vertex2D::stride
 * @return
 */
int Vertex2D::stride(){
    return sizeof(Vertex2D);
}
