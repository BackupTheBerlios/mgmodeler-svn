#include "openglwidget.h"
#include "view3d.h"
#include "trackball.h"
#include <qgl.h>
#include <iostream>

OpenglWidget::OpenglWidget (QWidget *parent, const char *name, 
			    bool ortho, bool trackball)
  :QGLWidget (parent, name), m_trackball_zoom (TRACKBALL_ZOOM_DEF)
{
  m_parent = parent;
  m_orthoview = ortho;
  m_update_projection = true;
  setMouseTracking (true);
  m_trackball_enable = trackball;
  if (m_trackball_enable)
    {
      m_trackball = new Trackball (100, 100);
      m_trackball_zoom = TRACKBALL_ZOOM_DEF;
      m_update_modelview = true;
    }
}

void 
OpenglWidget::initializeGL ()
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glMatrixMode (GL_MODELVIEW);
  glEnable (GL_DEPTH_TEST);
}

void 
OpenglWidget::clearGL ()
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void 
OpenglWidget::resizeGL (int w, int h)
{
  if (m_trackball_enable)
    m_trackball->setScreenSize (w, h);

  m_width = w;
  m_height = h;

  m_update_modelview = true;
  m_update_projection = true;

}

void 
OpenglWidget::paintGL ()
{
  View3D *v = static_cast<View3D *>(m_parent);
  std::vector<PluginObject *> plugins = v->getPlugins ();
  std::vector<PluginObject *>::iterator i;
  std::vector<PluginObject *>::iterator end=plugins.end();

  SyncContext ();  
  clearGL ();
  for (i = plugins.begin (); i!=end; ++i)
    (*i)->display ();
  
  swapBuffers ();
}


void
OpenglWidget::mousePressEvent (QMouseEvent *e) {
  View3D *parent = (View3D *)m_parent;
  parent-> parseMousePress (e);

  switch (e->button ()) {
  case QMouseEvent::LeftButton:
    if (m_trackball_enable)
      {
	m_trackball->startRotation (e->x (), e-> y());
	m_update_modelview = true;
	updateGL ();
      }
    break;
  default:
    break;
  }
}

void
OpenglWidget::mouseReleaseEvent (QMouseEvent *e) {
  //View3D *parent = (View3D *)m_parent;
  //parent-> parseMousePress (e);

  switch (e->button ()) {
  case QMouseEvent::LeftButton:
    if (m_trackball_enable)
      {
	m_trackball->stopRotation ();
	m_update_modelview = true;
	updateGL ();
      }
    break;
  default:
    break;
  }
}


void
OpenglWidget::mouseMoveEvent (QMouseEvent *e) {
  View3D *parent = (View3D *)m_parent;
  parent-> parseMouseMove (e);
  
  if (e-> state ()&QMouseEvent::LeftButton)
    if (m_trackball_enable)
      {
	m_trackball->computeOrientation (e->x (), e->y ());
	m_update_modelview = true;
	updateGL ();
      }
  
}

void
OpenglWidget::wheelEvent (QWheelEvent * e)
{ 
  e->accept ();
  if (e->delta ()<0)
    m_trackball_zoom *= 0.9;
  else
    m_trackball_zoom *= 1.1;
  
  m_update_modelview = true;
  updateGL ();
}


void
OpenglWidget::SyncContext ()
{
  if (m_update_projection)
    {
      m_update_projection = false;
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      
      float ratio = (float)m_width / (float)m_height;

      if (m_orthoview)
	gluOrtho2D (-1.0, 1.0, -1.0/ratio, 1.0/ratio);
      else
	gluPerspective (60.0, (double)m_width/(double)m_height, 0.1, 100.0);
      
      glViewport (0, 0, (GLint)m_width, (GLint)m_height);
      glMatrixMode (GL_MODELVIEW);
    } 

  if (m_trackball_enable)
    if (m_update_modelview)
      {
	Matrix4f modelview;

	m_update_modelview = false;
	
	m_trackball->getRotation (modelview);
	/* Send the Matrix */
	glLoadIdentity ();
	gluLookAt (0, 0, 2, 0, 0, 0, 0, 1, 0);
	glMultMatrixf ((float *)modelview[0]);
	glScalef (m_trackball_zoom, m_trackball_zoom, m_trackball_zoom);
      }
}
