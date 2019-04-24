#include "meshviewerwidget.h"

MeshViewerWidget::MeshViewerWidget(QWidget*_parent) : QGLWidget(_parent)
{
    triToDraw = 0;
    linesToDraw = 0;
    pointsToDraw = 0;

    setMouseTracking(true);
    setFocus();
}

MeshViewerWidget::MeshViewerWidget( QGLFormat& _fmt, QWidget* _parent ) : QGLWidget( _fmt, _parent )
{
    setMouseTracking(true);
    setFocus();
}

void MeshViewerWidget::initializeGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable( GL_DEPTH_TEST );

    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix_);
    set_scene_pos(Vec3f(0.0, 0.0, 0.0), 1.0);

    glEnable( GL_MULTISAMPLE );
}

void MeshViewerWidget::translate( const OpenMesh::Vec3f& _trans )
{
    glLoadIdentity();
    glTranslated( _trans[0], _trans[1], _trans[2] );
    glMultMatrixd( modelview_matrix_ );
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview_matrix_);
}

void MeshViewerWidget::resizeGL( int _w, int _h )
{
    update_projection_matrix();
    glViewport(0, 0, _w, _h);
    updateGL();
}

void MeshViewerWidget::reloadPOV()
{
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix_);
    set_scene_pos(Vec3f(0.0, 0.0, 0.0), 1.0);
    updateGL();
}

void MeshViewerWidget::loadMesh(GLfloat* verts, GLfloat* colors, int nVerts, GLuint* triangles, int nTriangles)
{
    GLfloat* vertsColsArray = new GLfloat[nVerts * 2];

    for(int i = 0; i < nVerts; i = i + 3)
    {
        int j = 2 * i;
        vertsColsArray[j] = colors[i] / 255.0;
        vertsColsArray[j+1] = colors[i+1] / 255.0;
        vertsColsArray[j+2] = colors[i+2] / 255.0;

        vertsColsArray[j+3] = verts[i];
        vertsColsArray[j+4] = verts[i+1];
        vertsColsArray[j+5] = verts[i+2];
    }

    glGenBuffers( 2, TriDataBuffers );

    glBindBuffer(GL_ARRAY_BUFFER, TriDataBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, nVerts * 2 * sizeof(GLfloat), vertsColsArray, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TriDataBuffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTriangles * sizeof(GLuint), triangles, GL_STATIC_DRAW);

    triToDraw = nTriangles;

    init = true;

    delete[] vertsColsArray;

    updateGL();
}

void MeshViewerWidget::loadLines(GLfloat* verts, GLfloat* colors, int nVerts, GLuint* lines, int nLines, QList<QPair<float, int> > es)
{
    GLfloat* linesColsArray = new GLfloat[nVerts * 2];

    for(int i = 0; i < nVerts; i = i + 3)
    {
        int j = 2 * i;
        linesColsArray[j] = colors[i] / 255.0;
        linesColsArray[j+1] = colors[i+1] / 255.0;
        linesColsArray[j+2] = colors[i+2] / 255.0;

        linesColsArray[j+3] = verts[i];
        linesColsArray[j+4] = verts[i+1];
        linesColsArray[j+5] = verts[i+2];
    }

    glGenBuffers( 2, LinesDataBuffers );


    glBindBuffer(GL_ARRAY_BUFFER, LinesDataBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, nVerts * 2 * sizeof(GLfloat), linesColsArray, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LinesDataBuffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nLines * sizeof(GLuint), lines, GL_STATIC_DRAW);

    linesToDraw = nLines;

    edgeSizes = es;

    delete[] linesColsArray;

    updateGL();
}

void MeshViewerWidget::loadPoints(GLfloat* verts, GLfloat* colors, int nVerts, GLuint* points, int nPoints, QList<QPair<float, int> > vs)
{
    GLfloat* pointsColsArray = new GLfloat[nVerts * 2];

    for(int i = 0; i < nVerts; i = i + 3)
    {
        int j = 2 * i;
        pointsColsArray[j] = colors[i] / 255.0;
        pointsColsArray[j+1] = colors[i+1] / 255.0;
        pointsColsArray[j+2] = colors[i+2] / 255.0;

        pointsColsArray[j+3] = verts[i];
        pointsColsArray[j+4] = verts[i+1];
        pointsColsArray[j+5] = verts[i+2];
    }

    glGenBuffers( 2, PointsDataBuffers );


    glBindBuffer(GL_ARRAY_BUFFER, PointsDataBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, nVerts * 2 * sizeof(GLfloat), pointsColsArray, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PointsDataBuffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nPoints * sizeof(GLuint), points, GL_STATIC_DRAW);

    pointsToDraw = nPoints;

    vertsSizes = vs;

    delete[] pointsColsArray;

    updateGL();
}


void MeshViewerWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_PROJECTION );
    glLoadMatrixd( projection_matrix_ );
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( modelview_matrix_ );

    if(triToDraw != 0)
    {
        glPolygonOffset(1.0, 2);

        // on charge le buffer 0 : une liste de vertex [r, g, b, x, y, z] (6 float)
        glBindBuffer(GL_ARRAY_BUFFER, TriDataBuffers[0]);

        // on charge la partie [r, g, b]
        glColorPointer( 3, GL_FLOAT, 6 * sizeof(float), 0 );
        // on charge la partie [x, y, z]
        glVertexPointer( 3, GL_FLOAT, 6 * sizeof(float), ((float*)NULL + (3)) );


        // on charge le buffer 1 : une liste d'ID [v0, v1, v2] (3 int)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TriDataBuffers[1]);

        glEnableClientState( GL_VERTEX_ARRAY );

        // affiche les faces en GL_COLOR_ARRAY
        glEnableClientState( GL_COLOR_ARRAY );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        glEnable(GL_POLYGON_OFFSET_FILL);
        glDrawElements(GL_TRIANGLES, triToDraw, GL_UNSIGNED_INT, 0);
        glDisable(GL_POLYGON_OFFSET_FILL);

        glDisableClientState( GL_COLOR_ARRAY );
        glDisableClientState( GL_VERTEX_ARRAY );
    }


    if(linesToDraw != 0)
    {
        // on charge le buffer 0 : une liste de vertex [r, g, b, x, y, z] (6 float)
        glBindBuffer(GL_ARRAY_BUFFER, LinesDataBuffers[0]);

        // on charge la partie [r, g, b]
        glColorPointer( 3, GL_FLOAT, 6 * sizeof(float), 0 );
        // on charge la partie [x, y, z]
        glVertexPointer( 3, GL_FLOAT, 6 * sizeof(float), ((float*)NULL + (3)) );

        // on charge le buffer 1 : une liste d'ID [v0, v1] (2 int)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LinesDataBuffers[1]);

        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );

        int cur = 0;
        for(int i = 0; i < edgeSizes.count(); i++)
        {
            glLineWidth(edgeSizes.at(i).first);
            glDrawElements(GL_LINES, edgeSizes.at(i).second*2, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLfloat) * cur));
            cur = cur + edgeSizes.at(i).second*2;
        }

        glDisableClientState( GL_COLOR_ARRAY );
        glDisableClientState( GL_VERTEX_ARRAY );
    }

    if(pointsToDraw != 0)
    {
        // on charge le buffer 0 : une liste de vertex [r, g, b, x, y, z] (6 float)
        glBindBuffer(GL_ARRAY_BUFFER, PointsDataBuffers[0]);

        // on charge la partie [r, g, b]
        glColorPointer( 3, GL_FLOAT, 6 * sizeof(float), 0 );
        // on charge la partie [x, y, z]
        glVertexPointer( 3, GL_FLOAT, 6 * sizeof(float), ((float*)NULL + (3)) );

        // on charge le buffer 1 : une liste d'ID [v0] (1 int)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PointsDataBuffers[1]);

        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );

        int cur = 0;
        for(int i = 0; i < vertsSizes.count(); i++)
        {
            glPointSize(vertsSizes.at(i).first);
            glDrawElements(GL_POINTS, vertsSizes.at(i).second, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLfloat) * cur));
            cur = cur + vertsSizes.at(i).second;
        }

        glDisableClientState( GL_COLOR_ARRAY );
        glDisableClientState( GL_VERTEX_ARRAY );
    }
}


void MeshViewerWidget::set_scene_pos( const OpenMesh::Vec3f& _cog, float _radius )
{
    center_ = _cog;
    radius_ = _radius;
    update_projection_matrix();
    view_all();
}

void MeshViewerWidget::update_projection_matrix()
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat) width() / (GLfloat) height(), 0.01*radius_, 100.0*radius_);
    glGetDoublev( GL_PROJECTION_MATRIX, projection_matrix_);
    glMatrixMode( GL_MODELVIEW );
}

void MeshViewerWidget::view_all()
{
    translate( Vec3f( -(modelview_matrix_[0]*center_[0] + modelview_matrix_[4]*center_[1] + modelview_matrix_[8]*center_[2] + modelview_matrix_[12]), -(modelview_matrix_[1]*center_[0] + modelview_matrix_[5]*center_[1] + modelview_matrix_[9]*center_[2] + modelview_matrix_[13]), -(modelview_matrix_[2]*center_[0] + modelview_matrix_[6]*center_[1] + modelview_matrix_[10]*center_[2] + modelview_matrix_[14] + 3.0*radius_) ) );
}

void MeshViewerWidget::mousePressEvent( QMouseEvent* _event )
{
    last_point_ok_ = map_to_sphere( last_point_2D_=_event->pos(), last_point_3D_ );
}

void MeshViewerWidget::mouseMoveEvent( QMouseEvent* _event )
{
    QPoint newPoint2D = _event->pos();

    Vec3f  newPoint3D;
    bool   newPoint_hitSphere = map_to_sphere( newPoint2D, newPoint3D );

    float dx = newPoint2D.x() - last_point_2D_.x();
    float dy = newPoint2D.y() - last_point_2D_.y();

    float w  = width();
    float h  = height();

    if ( (_event->buttons() == (Qt::LeftButton+Qt::MidButton)) ||  (_event->buttons() == Qt::LeftButton && _event->modifiers() == Qt::ControlModifier))
    {
        float value_y = radius_ * dy * 3.0 / h;
        translate(Vec3f(0.0, 0.0, value_y));
    }
    else if ( (_event->buttons() == Qt::MidButton) || (_event->buttons() == Qt::LeftButton && _event->modifiers() == Qt::AltModifier) )
    {
        float z = - (modelview_matrix_[ 2]*center_[0] + modelview_matrix_[ 6]*center_[1] + modelview_matrix_[10]*center_[2] + modelview_matrix_[14]) / (modelview_matrix_[ 3]*center_[0] + modelview_matrix_[ 7]*center_[1] + modelview_matrix_[11]*center_[2] + modelview_matrix_[15]);
        float aspect     = w / h;
        float near_plane = 0.01 * radius_;
        float top        = tan(45.0f/2.0f*M_PI/180.0f) * near_plane;
        float right      = aspect*top;
        translate(Vec3f( 2.0*dx/w*right/near_plane*z, -2.0*dy/h*top/near_plane*z, 0.0f));
    }
    else if (_event->buttons() == Qt::LeftButton)
    {
        if (last_point_ok_)
        {
            if ((newPoint_hitSphere = map_to_sphere(newPoint2D, newPoint3D)))
            {
                Vec3f axis = last_point_3D_ % newPoint3D;
                if (axis.sqrnorm() < 1e-7)
                    axis = Vec3f(1, 0, 0);
                else
                    axis.normalize();
                Vec3f d = last_point_3D_ - newPoint3D;
                float t = 0.5 * d.norm() / TRACKBALL_RADIUS;
                if (t < -1.0)
                    t = -1.0;
                else if (t > 1.0)
                    t = 1.0;
                float phi = 2.0 * asin(t);
                float angle = phi * 180.0 / M_PI;
                rotate(axis, angle);
            }
        }

    }
    last_point_2D_ = newPoint2D;
    last_point_3D_ = newPoint3D;
    last_point_ok_ = newPoint_hitSphere;
    updateGL();
}

void MeshViewerWidget::mouseReleaseEvent( QMouseEvent* /* _event */ )
{
    last_point_ok_ = false;
}

void MeshViewerWidget::wheelEvent(QWheelEvent* _event)
{
    float d = -(float)_event->delta() / 120.0 * 0.2 * radius_;
    translate(Vec3f(0.0, 0.0, d));
    updateGL();
    _event->accept();
}

bool MeshViewerWidget::map_to_sphere( const QPoint& _v2D, OpenMesh::Vec3f& _v3D )
{
    double x =  (2.0*_v2D.x() - width())/width();
    double y = -(2.0*_v2D.y() - height())/height();
    double xval = x;
    double yval = y;
    double x2y2 = xval*xval + yval*yval;

    const double rsqr = TRACKBALL_RADIUS*TRACKBALL_RADIUS;
    _v3D[0] = xval;
    _v3D[1] = yval;
    if (x2y2 < 0.5*rsqr)
        _v3D[2] = sqrt(rsqr - x2y2);
    else
        _v3D[2] = 0.5*rsqr/sqrt(x2y2);

    return true;
}

void MeshViewerWidget::rotate( const OpenMesh::Vec3f& _axis, float _angle )
{
    Vec3f t( modelview_matrix_[0]*center_[0] + modelview_matrix_[4]*center_[1] + modelview_matrix_[8]*center_[2] + modelview_matrix_[12], modelview_matrix_[1]*center_[0] + modelview_matrix_[5]*center_[1] + modelview_matrix_[9]*center_[2] + modelview_matrix_[13], modelview_matrix_[2]*center_[0] + modelview_matrix_[6]*center_[1] + modelview_matrix_[10]*center_[2] + modelview_matrix_[14] );
    glLoadIdentity();
    glTranslatef(t[0], t[1], t[2]);
    glRotated( _angle, _axis[0], _axis[1], _axis[2]);
    glTranslatef(-t[0], -t[1], -t[2]);
    glMultMatrixd(modelview_matrix_);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix_);
}
