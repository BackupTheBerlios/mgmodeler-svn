#include "openglwidget.h"
#include "view3d.h"
#include <qgl.h>
#include <iostream>

OpenglWidget::OpenglWidget (QWidget *parent, const char *name)
  :QGLWidget (parent, name)
{
  m_parent = parent;
  m_orthoview = false;
  setMouseTracking (true);
}

void 
OpenglWidget::initializeGL ()
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glMatrixMode (GL_MODELVIEW);
}

void 
OpenglWidget::clearGL ()
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void 
OpenglWidget::resizeGL (int w, int h)
{
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
      
  if (m_orthoview)
    gluOrtho2D (0, w, h, 0);
  else
    gluPerspective (60.0, (double)w/(double)h, 0.1, 100.0);
    
  glViewport (0, 0, (GLint)w, (GLint)h);
  glMatrixMode (GL_MODELVIEW);

  m_width = w;
  m_height = h;
}

void 
OpenglWidget::paintGL ()
{
  clearGL ();
  glBegin (GL_TRIANGLES);
  
  glColor3f (1.0, 0.0, 0.0);
  glVertex3f (0.0, 0.0, 0.0);
  glVertex3f (0.0, 2.0, 0.0);
  glVertex3f (2.0, 0.0, 0.0);

  glColor3f (0.0, 1.0, 0.0);
  glVertex3f (0.0, 0.0, 0.0);
  glVertex3f (0.0, 2.0, 0.0);
  glVertex3f (0.0, 0.0, 2.0);

  glColor3f (0.0, 0.0, 1.0);
  glVertex3f (0.0, 0.0, 0.0);
  glVertex3f (2.0, 0.0, 0.0);
  glVertex3f (0.0, 0.0, 2.0);


  glEnd ();
  swapBuffers ();
}


void
OpenglWidget::mousePressEvent (QMouseEvent *e) {
  View3D *parent = (View3D *)m_parent;
  parent-> parseMousePress (e);
}


void
OpenglWidget::mouseMoveEvent (QMouseEvent *e) {
  View3D *parent = (View3D *)m_parent;
  parent-> parseMouseMove (e);
}



