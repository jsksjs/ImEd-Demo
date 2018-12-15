#include "glsheet.h"


/**
 * @brief GLSheet::GLSheet
 * @param parent
 * @param wh
 * @param primary
 * @param secondary
 */
GLSheet::GLSheet(QWidget *parent, const QSize wh, const QVector4D primary, const QVector4D secondary)
    : QOpenGLWidget(parent), f(), m_vertex(QOpenGLBuffer::VertexBuffer), m_index(QOpenGLBuffer::IndexBuffer),
    lastMouseVertex(QVector2D(0,0), QVector4D(0,0,0,0)),
    cam(QVector3D(0,0,0), 0, this->width(), this->height(), 0, 1, -1)
{
    // TODO: FIX ALPHA BETWEEN TEXTURES
    // ADD DUPLICATE LAYER
    // ADD "RAISE" LAYER
    // ADD "LOWER" LAYER
    // CREATE "MESH" OBJECT TO STORE VERTEX VECTORS WITH DATA THAT KEEPS TRACK OF A BOUNDING BOX FOR THE MESH (BASED ON POSITION AND OFFSET)
    // FIGURE OUT BETTER WAY OF ORGANIZING SHADERS
    // FIGURE OUT BETTER WAY OF ORGANIZING TEXTURES AND QUADS (LAYERS)
    // FIX ALPHA
    // CIRCLE BRUSH
    // BRUSH PREVIEW (OUTLINE)
    // ADD POPOUT MENUS TO "PIN" ACTIONS THAT ALLOW FOR "TOP | MIDDLE | BOTTOM" PINNINGS EACH FOR LEFT, RIGHT, CENTER OPTION
    originalSize = wh;
    actualSize = QSize(static_cast<int>(originalSize.width() * trans.GetScale().x()), static_cast<int>(originalSize.height() * trans.GetScale().y()));
    drawRGBA = primary;
    secondaryRGBA = secondary;

    const QVector4D temp(1.0f, 1.0f, 1.0f, 0.0f);
    layerTarget = {Vertex2D(QVector2D(0, 0), temp, QVector2D(0, 0), QVector2D(0.0f, 1.0f)),
               Vertex2D(QVector2D(0, originalSize.height()), temp, QVector2D(0, 0), QVector2D(0.0f, 0.0f)),
               Vertex2D(QVector2D(originalSize.width(), 0), temp, QVector2D(0, 0), QVector2D(1.0f, 1.0f)),
               Vertex2D(QVector2D(originalSize.width(), originalSize.height()), temp, QVector2D(0, 0), QVector2D(1.0f, 0.0f))};

    backTarget = {Vertex2D(QVector2D(0, 0), temp, QVector2D(0, 0), QVector2D(0.0f, originalSize.height()/8)),
              Vertex2D(QVector2D(0, originalSize.height()), temp, QVector2D(0, 0), QVector2D(0.0f, 0.0f)),
              Vertex2D(QVector2D(originalSize.width(), 0), temp, QVector2D(0, 0), QVector2D(originalSize.width()/8, originalSize.height()/8)),
              Vertex2D(QVector2D(originalSize.width(), originalSize.height()), temp, QVector2D(0, 0), QVector2D(originalSize.width()/8, 0.0f))};

    layerTargetIndex = {0, 2, 1, 3, 1, 2};

    fillGrid(0.025f, QVector4D(0.8f, 0.8f, 0.8f, 0.4f));
    currentLayer = layers.end();


}

/**
 * @brief GLSheet::~GLSheet
 */
GLSheet::~GLSheet()
{
    makeCurrent();
    currentIndex.clear();
    currentMesh.clear();
    currentMeshInd.clear();
    m_vertex.destroy();
    m_index.destroy();
    m_object.destroy();
    for(const QOpenGLShaderProgram *s : m_program) {
        delete s;
    }
    delete m_fbo;
    doneCurrent();
}

/**
 * @brief GLSheet::secondaryColor
 * @param secondary
 */
void GLSheet::secondaryColor(const QVector4D secondary)
{
    secondaryRGBA = secondary;
}

/**
 * @brief GLSheet::drawColor
 * @param primary
 */
void GLSheet::drawColor(const QVector4D primary)
{
    drawRGBA = primary;
}

/**
 * @brief GLSheet::getOriginalSize
 * @return
 */
QSize GLSheet::getOriginalSize() const
{
    return originalSize;
}

/**
 * @brief GLSheet::getActualSize
 * @return
 */
QSize GLSheet::getActualSize() const
{
    return actualSize;
}

/**
 * @brief GLSheet::glCoordinates
 * @param position Mouse position (widget-space)
 * @param integers Return integers?
 * @return Position in world-space
 */
QVector2D GLSheet::glCoordinates(const QPoint position, const bool integers)
{
    QVector2D point = QVector3D(position.x() - (trans.GetPos().x() * (trans.GetScale().x()-1)),
                                (this->height() - 1.0f - position.y()) + (trans.GetPos().y() * (trans.GetScale().y()-1)), 0)
                      .unproject(trans.GetModel(), cam.GetViewProjection(),
                                 QRect(static_cast<int>(trans.GetPos().x()), static_cast<int>(-trans.GetPos().y()), this->width(), this->height())).toVector2D();
    if(integers) {
        point.setX(floorf(point.x()));
        point.setY(ceilf(point.y()));
    }
    return point;
}

/**
 * @brief GLSheet::screenCoordinates
 * @param position
 * @return
 */
QVector2D GLSheet::screenCoordinates(const QPoint position)
{
    QVector2D point = QVector3D(position.x(), -position.y() - 1.0f + this->height(), 0).unproject(trans.GetModel(), cam.GetViewProjection(),
                                                                                                  QRect(0, 0, this->width(), this->height())).toVector2D();
    point.setX(floorf(point.x())+1);
    point.setY(ceilf(point.y()));

    return point;
}

/**
 * @brief GLSheet::getPosDelta
 * @return
 */
QPoint GLSheet::getPosDelta()
{
    return trans.GetPos().toPoint();
}

/**
 * @brief GLSheet::fitScreen
 */
void GLSheet::fitScreen()
{
    if((this->width() - originalSize.width()) / double(this->width()) < (this->height() - originalSize.height()) / double(this->height()))
        scale(this->width() / double(originalSize.width()));
    else
        scale(this->height() / double(originalSize.height()));
    moveToCenter();
}

/**
 * @brief GLSheet::moveToCenter
 */
void GLSheet::moveToCenter()
{
    setPos(geometry().center() - QPoint(actualSize.width(), actualSize.height())/2);
}

/**
 * @brief GLSheet::moveToLeft
 */
void GLSheet::moveToLeft()
{
    setPos(QPoint(0, geometry().center().y() - actualSize.height()/2));
}

/**
 * @brief GLSheet::moveToRight
 */
void GLSheet::moveToRight()
{
    setPos(QPoint(geometry().right() - actualSize.width(),
                  geometry().center().y() - actualSize.height()/2));
}

/**
 * @brief GLSheet::translate
 * @param p
 */
void GLSheet::translate(const QPoint &p)
{
    trans.SetPos(QVector3D(int(trans.GetPos().x() + p.x()), int(trans.GetPos().y() + p.y()), 0));
}

/**
 * @brief GLSheet::setPos
 * @param p
 */
void GLSheet::setPos(const QPoint &p)
{
    trans.SetPos(QVector3D(p.x(), p.y(), 0));
}

/**
 * @brief GLSheet::initializeGL
 */
void GLSheet::initializeGL()
{
    f.initializeOpenGLFunctions();
    m_fbo = new QOpenGLFramebufferObject(originalSize, QOpenGLFramebufferObject::NoAttachment, GL_TEXTURE_2D, GL_RGBA8);
    m_fbo->bind();
    glViewport(0,0, originalSize.width(), originalSize.height());
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_fbo->release();

    background = new QOpenGLTexture(QImage(":/assets/back.png").mirrored());
    background->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
    background->setWrapMode(QOpenGLTexture::Repeat);

    // SQUARE
    m_program.push_back(new QOpenGLShaderProgram());
    m_program.back()->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/square.vert");
    m_program.back()->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/square.frag");
    m_program.back()->link();
    m_program.back()->bind();

    // CIRCLE
    m_program.push_back(new QOpenGLShaderProgram());
    m_program.back()->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/circle.vert");
    m_program.back()->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/circle.frag");
    m_program.back()->link();
    m_program.back()->bind();

    // TEXTURE
    m_program.push_back(new QOpenGLShaderProgram());
    m_program.back()->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/background.vert");
    m_program.back()->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/background.frag");
    m_program.back()->link();
    m_program.back()->bind();

    m_vertex.create();
    m_vertex.bind();
    m_vertex.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    m_object.create();
    m_object.bind();

    m_index.create();
    m_index.bind();
    m_index.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    m_program[TEXTURE]->enableAttributeArray(0);
    m_program[TEXTURE]->enableAttributeArray(1);
    m_program[TEXTURE]->enableAttributeArray(2);
    m_program[TEXTURE]->setAttributeBuffer(0, GL_FLOAT, Vertex2D::positionOffset(), Vertex2D::PositionTupleSize, Vertex2D::stride());
    m_program[TEXTURE]->setAttributeBuffer(1, GL_FLOAT, Vertex2D::texCoordsOffset(), Vertex2D::TexCoordsTupleSize, Vertex2D::stride());
    m_program[TEXTURE]->setAttributeBuffer(2, GL_FLOAT, Vertex2D::offsetOffset(), Vertex2D::OffsetTupleSize, Vertex2D::stride());

    m_program[SQUARE]->enableAttributeArray(0);
    m_program[SQUARE]->enableAttributeArray(1);
    m_program[SQUARE]->enableAttributeArray(2);
    m_program[SQUARE]->setAttributeBuffer(0, GL_FLOAT, Vertex2D::positionOffset(), Vertex2D::PositionTupleSize, Vertex2D::stride());
    m_program[SQUARE]->setAttributeBuffer(1, GL_FLOAT, Vertex2D::colorOffset(), Vertex2D::ColorTupleSize, Vertex2D::stride());
    m_program[SQUARE]->setAttributeBuffer(2, GL_FLOAT, Vertex2D::offsetOffset(), Vertex2D::OffsetTupleSize, Vertex2D::stride());

    m_index.release();
    m_object.release();
    m_vertex.release();
    m_program[TEXTURE]->release();
    for(QOpenGLShaderProgram *s : m_program) {
        s->removeAllShaders();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    insertLayer();

    QColor back(55, 55, 55);
    glClearColor(static_cast<GLclampf>(back.redF()), static_cast<GLclampf>(back.greenF()), static_cast<GLclampf>(back.blueF()), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

/**
 * @brief GLSheet::paintGL
 */
void GLSheet::paintGL()
{
    if(currentLayer == layers.end())
        return;
    rendered = false;
    frameTime();
    m_vertex.bind(); {
        m_object.bind();
        m_index.bind(); {
            m_program[TEXTURE]->bind(); {
                glViewport(0, 0, width(), height());
                glClear(GL_COLOR_BUFFER_BIT);
                // Background
                glDisable(GL_BLEND);
                glBindTexture(GL_TEXTURE_2D, background->textureId());
                m_program[TEXTURE]->setAttributeBuffer(1, GL_FLOAT, Vertex2D::texCoordsOffset(), Vertex2D::TexCoordsTupleSize, Vertex2D::stride());
                m_program[TEXTURE]->setUniformValue(m_program[TEXTURE]->uniformLocation("sampler"), 0);
                m_program[TEXTURE]->setUniformValue(m_program[TEXTURE]->uniformLocation("modelviewprojection"), cam.GetViewProjection() * trans.GetModel());
                m_vertex.allocate(backTarget.data(), static_cast<int>(sizeof(backTarget.data()[0]) * backTarget.size()));
                m_index.allocate(layerTargetIndex.data(), static_cast<int>(sizeof(layerTargetIndex.data()[0]) * layerTargetIndex.size()));
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(layerTargetIndex.size()), GL_UNSIGNED_INT, nullptr);

                glEnable(GL_BLEND);
                // FIX
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Layers
                for(Layer l : layers) {
                    l.DrawTexture(&m_vertex);
                }
            }
            m_program[TEXTURE]->release();
            m_program[currentBrushShader]->bind(); {
                m_program[currentBrushShader]->setAttributeBuffer(1, GL_FLOAT, Vertex2D::colorOffset(), Vertex2D::ColorTupleSize, Vertex2D::stride());
                m_program[currentBrushShader]->setUniformValue(m_program[currentBrushShader]->uniformLocation("modelviewprojection"),
                                                               Camera(QVector3D(0,0,0), 0, originalSize.width(),
                                                                      originalSize.height(), 0, 1, -1).GetViewProjection() * Transform(trans.GetPos()).GetModel());
                //DRAWING
                f.glBlendFuncSeparate(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
                f.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glViewport(0,0, originalSize.width(), originalSize.height());
                (*currentLayer).DrawBuffer(f, &m_vertex, &m_index, &currentMesh, &currentIndex);
                rendered = true;
                //currentTool->cleanUp();
                if(drawGrid && trans.GetScale().x() >= 20) {
                    m_program[currentBrushShader]->setUniformValue(m_program[currentBrushShader]->uniformLocation("modelviewprojection"),
                                                                   Camera(QVector3D(0,0,0), 0, width(),
                                                                          height(), 0, 1, -1).GetViewProjection() * trans.GetModel());
                    glViewport(0,0, width(), height());
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    m_vertex.allocate(grid.data(), static_cast<int>(sizeof(grid.data()[0]) * grid.size()));
                    m_index.allocate(gridIndex.data(), static_cast<int>(sizeof(gridIndex.data()[0]) * gridIndex.size()));
                    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gridIndex.size()), GL_UNSIGNED_INT, nullptr);
                }
            }
            m_program[currentBrushShader]->release();
        }
        m_index.release();
        m_object.release();
    }
    m_vertex.release();
    update();
}

/**
 * @brief GLSheet::resizeGL
 * @param w
 * @param h
 */
void GLSheet::resizeGL(const int w, const int h)
{
    glViewport(0, 0, w, h);
    cam = Camera(QVector3D(0,0,0), 0, w, h, 0, 1, -1);
}

/**
 * @brief GLSheet::mouseMoveEvent
 * @param event
 */
void GLSheet::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton) {
        QVector2D point = glCoordinates(event->pos());
        if(point != lastMouseVertex.position()) {
            drawBrush(point, drawRGBA);
        }
    }
    else if(event->buttons() == Qt::RightButton) {
        QVector2D point = glCoordinates(event->pos());
        if(point != lastMouseVertex.position()) {
            drawBrush(point, secondaryRGBA);
        }
    }
}

/**
 * @brief GLSheet::mouseReleaseEvent
 * @param event
 */
void GLSheet::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(!currentMesh.empty()) {
        draw = false;
    }
    currentIndex.clear();
    currentMesh.clear();
    currentMeshInd.clear();
    if(currentLayer != layers.end()) {
        makeCurrent();
        emit preview((*currentLayer).Image());
    }
}

/**
 * @brief GLSheet::mousePressEvent
 * @param event
 */
void GLSheet::mousePressEvent(QMouseEvent *event)
{
    QVector2D point = glCoordinates(event->pos());
    const int x = static_cast<int>(point.x());
    const int y = static_cast<int>(point.y());
    QVector4D color;
    lastMouseVertex = Vertex2D(QVector2D(x, y), color);
    if(event->buttons() == Qt::LeftButton) {
        color = drawRGBA;
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(x, y), color, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y-1), color, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize))});
    }
    else if(event->buttons() == Qt::RightButton) {
        color = secondaryRGBA;
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(x, y), color, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y-1), color, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize))});
    }
}

/**
 * @brief GLSheet::changeBrushShader
 * @param s
 */
void GLSheet::changeBrushShader(GLSheet::Shaders &s)
{
    currentBrushShader = s;
}

/**
 * @brief GLSheet::insertMesh
 * @param mesh
 * @param index
 * @param meshInd
 * @param v
 */
void GLSheet::insertMesh(std::vector<Vertex2D> &mesh, std::vector<unsigned int> &index,
                         std::map<const Vertex2D, unsigned int> &meshInd, const std::vector<Vertex2D> &v)
{
    for(const Vertex2D &vert : v) {
        const std::map<const Vertex2D, unsigned int>::const_iterator it = meshInd.find(vert);
        if(it != meshInd.end()) {            
            index.push_back(it->second);
        }
        else{
            meshInd.insert(std::pair<const Vertex2D, unsigned int>(vert, mesh.size()));
            index.push_back(static_cast<unsigned int>(mesh.size()));
            mesh.push_back(vert);
        }
    }
}

/**
 * @brief GLSheet::drawBrush
 * @param point
 * @param color
 */
void GLSheet::drawBrush(const QVector2D point, const QVector4D color)
{
    if(!currentMesh.empty() && draw) {
        if(rendered) {
            currentIndex.clear();
            currentMesh.clear();
            currentMeshInd.clear();
            rendered = false;
        }
        const QVector2D first = lastMouseVertex.position();
        if(!m_aa)
            BresenhamLine(first, point, color);
        else
            WuLine(first, point, color);
    }
    const int x = static_cast<int>(point.x());
    const int y = static_cast<int>(point.y());
    lastMouseVertex = Vertex2D(QVector2D(x, y), color);
    insertMesh(currentMesh, currentIndex, currentMeshInd,
               std::vector<Vertex2D> {Vertex2D(QVector2D(x, y), color, QVector2D(-m_brushSize, m_brushSize)),
                                      Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                      Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize)),
                                      Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                      Vertex2D(QVector2D(x+1, y-1), color, QVector2D(m_brushSize, -m_brushSize)),
                                      Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize))});
    draw = true;
}

/**
 * @brief GLSheet::frameTime
 */
void GLSheet::frameTime()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
    frames++;
    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count() >= 1.0) {
        emit frameInfo(QString(tr("%1 ms/frame, %2 fps ")).arg(QString::number(1000.0/double(frames)).trimmed(), QString::number(frames).trimmed()));
        frames = 0;
        lastTime = std::chrono::high_resolution_clock::now();
    }
}

/**
 * @brief GLSheet::BresenhamLineLow
 * @param first
 * @param second
 * @param color
 */
void GLSheet::BresenhamLineLow(const QVector2D &first, const QVector2D &second, const QVector4D &color)
{
    int dx = static_cast<int>(second.x() - first.x());
    int dy = static_cast<int>(second.y() - first.y());
    short yi = 1;
    if(dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = 2 * dy - dx;
    int y = static_cast<int>(first.y());
    for(int x = static_cast<int>(first.x()); x < second.x(); x++) {
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(x, y), color, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y-1), color, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize))});
        if(D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

/**
 * @brief GLSheet::BresenhamLineHigh
 * @param first
 * @param second
 * @param color
 */
void GLSheet::BresenhamLineHigh(const QVector2D &first, const QVector2D &second, const QVector4D &color)
{
    int dx = static_cast<int>(second.x() - first.x());
    int dy = static_cast<int>(second.y() - first.y());
    short xi = 1;
    if(dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = 2 * dx - dy;
    int x = static_cast<int>(first.x());
    for(int y = static_cast<int>(first.y()); y < second.y(); y++) {
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(x, y), color, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(x, y-1), color, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y-1), color, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(x+1, y), color, QVector2D(m_brushSize, m_brushSize))});
        if(D > 0) {
            x += xi;
            D -= 2 * dy;
        }
        D += 2 * dx;
    }
}

/**
 * @brief GLSheet::fpart
 * @param x
 * @return
 */
float GLSheet::fpart(const float x)
{
    return x - floorf(x);
}

/**
 * @brief GLSheet::rfpart
 * @param x
 * @return
 */
float GLSheet::rfpart(const float x)
{
    return 1 - fpart(x);
}

/**
 * @brief GLSheet::fillGrid
 * @param width
 * @param color
 */
void GLSheet::fillGrid(const float width, const QVector4D color)
{
    grid.clear();
    gridIndex.clear();
    gridMeshInd.clear();
    const QVector4D col = color;
    // horizontal
    const QVector2D origin = QVector2D(0,0);
    const QVector2D end = QVector2D(originalSize.width(), originalSize.height());
    for(int y = static_cast<int>(origin.y()); y <= end.y(); y++) {
        insertMesh(grid, gridIndex, gridMeshInd,
                   {Vertex2D(QVector2D(origin.x(), y), col, QVector2D(-width, -width)),
                    Vertex2D(QVector2D(end.x(), y), col, QVector2D(0, -width)),
                    Vertex2D(QVector2D(origin.x(), y), col, QVector2D(-width, width)),
                    Vertex2D(QVector2D(end.x(), y), col, QVector2D(0, width)),
                    Vertex2D(QVector2D(origin.x(), y), col, QVector2D(-width, width)),
                    Vertex2D(QVector2D(end.x(), y), col, QVector2D(0, -width))});
    }
    // vertical
    for(int x = static_cast<int>(origin.x()); x <= end.x(); x++) {
        insertMesh(grid, gridIndex, gridMeshInd,
                   {Vertex2D(QVector2D(x, origin.y()), col, QVector2D(width, 0)),
                    Vertex2D(QVector2D(x, end.y()), col, QVector2D(width, 0)),
                    Vertex2D(QVector2D(x, origin.y()), col, QVector2D(-width, 0)),
                    Vertex2D(QVector2D(x, end.y()), col, QVector2D(-width, 0)),
                    Vertex2D(QVector2D(x, origin.y()), col, QVector2D(-width, 0)),
                    Vertex2D(QVector2D(x, end.y()), col, QVector2D(width, 0))});
    }
}

/**
 * @brief GLSheet::insertLayer
 */
void GLSheet::insertLayer()
{
    makeCurrent();
    if(currentLayer != layers.end()) {
        layers.insert(std::next(currentLayer), Layer(originalSize));
    }
    else{
        layers.push_back(Layer(originalSize));
        currentLayer = layers.begin();
    }
}

/**
 * @brief GLSheet::duplicateLayer
 */
void GLSheet::duplicateLayer()
{
    makeCurrent();
    layers.insert(std::next(currentLayer), Layer(*currentLayer, &f));
}

/**
 * @brief GLSheet::deleteLayer
 * @param pos
 */
void GLSheet::deleteLayer(const int pos)
{
    makeCurrent();
    if(currentLayer != layers.end()){
        layers.erase(std::prev(layers.end(), pos));
    }
}

/**
 * @brief GLSheet::swapLayer
 * @param pos
 */
void GLSheet::swapLayer(const int pos)
{
    makeCurrent();
    std::swap(*currentLayer, *std::prev(layers.end(), (pos+1)));
}

/**
 * @brief GLSheet::setCurrentLayer
 * @param pos
 */
void GLSheet::setCurrentLayer(const int pos)
{
    currentLayer = std::prev(layers.end(), (pos+1));
}

/**
 * @brief GLSheet::setGridEnabled
 * @param e
 */
void GLSheet::setGridEnabled(const bool e)
{
    drawGrid = e;
}

/**
 * @brief GLSheet::WuLine
 * @param first
 * @param second
 * @param color
 */
void GLSheet::WuLine(QVector2D first, QVector2D second, const QVector4D color)
{
    const bool steep = std::abs(second.y() - first.y()) > std::abs(second.x() - first.x());

    if(steep) {
        float temp = first.x();
        first.setX(first.y());
        first.setY(temp);

        temp = second.x();
        second.setX(second.y());
        second.setY(temp);
    }

    if(first.x() > second.x()) {
        float temp = first.x();
        first.setX(second.x());
        second.setX(temp);

        temp = first.y();
        first.setY(second.y());
        second.setY(temp);
    }

    const float dx = second.x() - first.x();
    const float dy = second.y() - first.y();
    float gradient;
    if(dx == 0.0f)
        gradient = 1.0f;
    else
        gradient = dy / dx;

    int xend = static_cast<int>(roundf(first.x()));
    float yend = first.y() + gradient * (xend - first.x());

    float xgap = rfpart(first.x() + 0.5f);

    const int xpxl1 = xend;
    const int ypxl1 = static_cast<int>(floor(yend));

    if(steep) {
        QVector4D tcolor = color;
        tcolor.setW(color.w() * rfpart(yend) * xgap);
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(ypxl1, xpxl1), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(ypxl1, xpxl1-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl1+1, xpxl1), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(ypxl1, xpxl1-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl1+1, xpxl1-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl1+1, xpxl1), tcolor, QVector2D(m_brushSize, m_brushSize))});

        tcolor.setW(color.w() * fpart(yend) * xgap);
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(1+ypxl1, xpxl1), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(1+ypxl1, xpxl1-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl1+2, xpxl1), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(1+ypxl1, xpxl1-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl1+2, xpxl1-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl1+2, xpxl1), tcolor, QVector2D(m_brushSize, m_brushSize))});
    }
    else{
        QVector4D tcolor = color;
        tcolor.setW(color.w() * rfpart(yend) * xgap);
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(xpxl1, ypxl1), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1, ypxl1-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1+1, ypxl1), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1, ypxl1-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1+1, ypxl1-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1+1, ypxl1), tcolor,  QVector2D(m_brushSize, m_brushSize))});

        tcolor.setW(color.w() * fpart(yend) * xgap);
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(xpxl1, 1+ypxl1), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1, 1+ypxl1-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1+1, 1+ypxl1), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1, 1+ypxl1-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1+1, 1+ypxl1-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl1+1, 1+ypxl1), tcolor, QVector2D(m_brushSize, m_brushSize))});
    }

    float intery = yend + gradient;

    //second end point
    xend = std::lround(second.x());
    yend = second.y() + gradient * (xend - second.x());

    xgap = fpart(second.x() + 0.5f);

    const int xpxl2 = xend;
    const int ypxl2 = static_cast<int>(floor(yend));

    if(steep) {
        QVector4D tcolor = color;
        tcolor.setW(color.w() * rfpart(yend) * xgap);
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(ypxl2, xpxl2), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(ypxl2, xpxl2-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl2+1, xpxl2), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(ypxl2, xpxl2-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl2+1, xpxl2-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl2+1, xpxl2), tcolor, QVector2D(m_brushSize, m_brushSize))});

        tcolor.setW(color.w() * fpart(yend) * xgap);
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(1+ypxl2, xpxl2), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(1+ypxl2, xpxl2-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl2+2, xpxl2), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(1+ypxl2, xpxl2-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl2+2, xpxl2-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(ypxl2+2, xpxl2), tcolor, QVector2D(m_brushSize, m_brushSize))});
    }
    else{
        QVector4D tcolor = color;
        tcolor.setW(color.w() * rfpart(yend) * xgap);
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(xpxl2, ypxl2), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2, ypxl2-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2+1, ypxl2), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2, ypxl2-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2+1, ypxl2-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2+1, ypxl2), tcolor, QVector2D(m_brushSize, m_brushSize))});

        tcolor.setW(color.w() * fpart(yend) * xgap);
        insertMesh(currentMesh, currentIndex, currentMeshInd,
                   std::vector<Vertex2D> {Vertex2D(QVector2D(xpxl2, 1+ypxl2), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2, ypxl2), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2+1, 1+ypxl2), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2, ypxl2), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2+1, ypxl2), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                          Vertex2D(QVector2D(xpxl2+1, 1+ypxl2), tcolor, QVector2D(m_brushSize, m_brushSize))});
    }

    if(steep) {
        QVector4D tcolor = color;
        for(int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
            tcolor.setW(color.w() * rfpart(intery));
            insertMesh(currentMesh, currentIndex, currentMeshInd,
                       std::vector<Vertex2D> {Vertex2D(QVector2D(floorf(intery), x), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                              Vertex2D(QVector2D(floorf(intery), x-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(floorf(intery)+1, x), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                              Vertex2D(QVector2D(floorf(intery), x-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(floorf(intery)+1, x-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(floorf(intery)+1, x), tcolor, QVector2D(m_brushSize, m_brushSize))});

            tcolor.setW(color.w() * fpart(intery));
            insertMesh(currentMesh, currentIndex, currentMeshInd,
                       std::vector<Vertex2D> {Vertex2D(QVector2D(1+floorf(intery), x), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                              Vertex2D(QVector2D(1+floorf(intery), x-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(1+floorf(intery)+1, x), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                              Vertex2D(QVector2D(1+floorf(intery), x-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(1+floorf(intery)+1, x-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(1+floorf(intery)+1, x), tcolor, QVector2D(m_brushSize, m_brushSize))});
            intery += gradient;
        }
    }
    else{
        QVector4D tcolor = color;
        for(int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
            tcolor.setW(color.w() * rfpart(intery));
            insertMesh(currentMesh, currentIndex, currentMeshInd,
                       std::vector<Vertex2D> {Vertex2D(QVector2D(x, floorf(intery)), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                              Vertex2D(QVector2D(x, floorf(intery)-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(x+1, floorf(intery)), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                              Vertex2D(QVector2D(x, floorf(intery)-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(x+1, floorf(intery)-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(x+1, floorf(intery)), tcolor, QVector2D(m_brushSize, m_brushSize))});

            tcolor.setW(color.w() * fpart(intery));
            insertMesh(currentMesh, currentIndex, currentMeshInd,
                       std::vector<Vertex2D> {Vertex2D(QVector2D(x, 1+floorf(intery)), tcolor, QVector2D(-m_brushSize, m_brushSize)),
                                              Vertex2D(QVector2D(x, 1+floorf(intery)-1), tcolor,  QVector2D(-m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(x+1, 1+floorf(intery)), tcolor, QVector2D(m_brushSize, m_brushSize)),
                                              Vertex2D(QVector2D(x, 1+floorf(intery)-1), tcolor, QVector2D(-m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(x+1, 1+floorf(intery)-1), tcolor, QVector2D(m_brushSize, -m_brushSize)),
                                              Vertex2D(QVector2D(x+1, 1+floorf(intery)), tcolor, QVector2D(m_brushSize, m_brushSize))});
            intery += gradient;
        }
    }
}

/**
 * @brief GLSheet::BresenhamLine
 * @param first
 * @param second
 * @param color
 */
void GLSheet::BresenhamLine(const QVector2D &first, const QVector2D &second, const QVector4D &color)
{
    if(std::abs(second.y() - first.y()) < std::abs(second.x() - first.x())) {
        if(first.x() > second.x())
            BresenhamLineLow(second, first, color);
        else
            BresenhamLineLow(first, second, color);
    }
    else{
        if(first.y() > second.y())
            BresenhamLineHigh(second, first, color);
        else
            BresenhamLineHigh(first, second, color);
    }
}

/**
 * @brief GLSheet::scale
 * @param s
 */
void GLSheet::scale(const double s)
{
    trans.SetScale(QVector3D(static_cast<float>(s), static_cast<float>(s), 1));
    actualSize = originalSize * s;
    emit scaleChanged(s*100);
}

/**
 * @brief GLSheet::scalePercent
 * @param s
 */
void GLSheet::scalePercent(const double s)
{
    const double percent = s/100;
    trans.SetScale(QVector3D(static_cast<float>(percent), static_cast<float>(percent), 1));
    actualSize = originalSize * percent;
    emit scaleChanged(s);
}

/**
 * @brief GLSheet::brushSize
 * @param s
 */
void GLSheet::brushSize(const int s)
{
    m_brushSize = (s-1)/2;
}

/**
 * @brief GLSheet::antialiased
 * @param aa
 */
void GLSheet::antialiased(const bool aa)
{
    m_aa = aa;
}

/**
 * @brief GLSheet::parentMoved
 * @param p
 */
void GLSheet::parentMoved(const QPoint p)
{
    translate(p);
}

