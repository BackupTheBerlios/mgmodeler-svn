#include "view3d.h"
#include "openglwidget.h"
#include "trackball.h"
#include "plugin.h"

#include <qgl.h>
#include <qstring.h>
#include <iostream>

PluginObject *View3D::s_plugin_current = NULL;

View3D::View3D (QWorkspace *parent, std::string name)
  :QMainWindow (parent, name.c_str (), 0)
{
}


View3D::View3D (QWorkspace *parent, eView view)
  : QMainWindow (parent, "OpenGL 2D View", 0)
{
  m_glwidget = new OpenglWidget (this, "OpenGL 2D View", true, false);
  setCentralWidget (m_glwidget);
  m_view = view;
  m_cursor_x = m_cursor_y=0;
  m_editable = true;
  setMouseTracking (true);
  
  m_plugin_active = NULL;

  m_mode = MODE_EDIT;
}

void
View3D::setupView ()
{
  double distance = 3.0;

  glLoadIdentity ();
  
  switch (m_view)
    {
    case VIEW_PROFIL:
      setCaption ("OpenGL 2D View: Profil");
      break;
    case VIEW_SECTION:
      setCaption ("OpenGL 2D View: Section");
      break;
    case VIEW_WAY:
      setCaption ("OpenGL 2D View: Way");
      break;
    default:
      std::cout << "Warning Invalid Axe"<<std::endl;
      break;

    }
}

void
View3D::updateStatusBar (int x, int y)
{
  statusBar() ->clear ();
  
  statusBar() ->message (QString().setNum (x)+QString(" ")+
			 QString().setNum (y));
  
}


static void
unproject (int x, int y, double *dx, double *dy, double *dz)
{
  double mv[16];
  double pj[16];
  int vp[16];
  

  glGetDoublev (GL_MODELVIEW_MATRIX, mv);
  glGetDoublev (GL_PROJECTION_MATRIX, pj);
  glGetIntegerv (GL_VIEWPORT, vp);
  
  gluUnProject (x, y, 0,
              mv, pj, vp,
              dx, dy, dz);

}

void
View3D::parseMousePress (QMouseEvent *e)
{
  if (e->button () == QMouseEvent::LeftButton)
    if (s_plugin_current)
      {
	m_plugins. push_back (m_plugin_active);
	m_plugin_active = (PluginObject *)
	  s_plugin_current-> m_createinstance ();
	s_plugin_current = NULL;
      }

  double x, y, z;
  unproject (e->x(), e->y(), &x ,&y, &z);

  if (m_plugin_active)
    m_plugin_active->buttonDown (e->button(), x, y, z);
}

void
View3D::parseMouseRelease (QMouseEvent *e)
{
  double x, y, z;
  unproject (e->x(), e->y(), &x ,&y, &z);


  if (m_plugin_active)
    m_plugin_active->buttonUp (e->button(), x, y, z);
}

void
View3D::parseMouseMove (QMouseEvent *e)
{
  if (e->x ()>0 && e->y ()>0)
    {
      m_cursor_x = e-> x();
      m_cursor_y = e-> y();
      
      double x, y, z;
      unproject (e->x(), e->y(), &x ,&y, &z);


      updateStatusBar (x, y);
    }
}

void
View3D::setCurrentPlugin (PluginObject *p) 
{
  s_plugin_current = p;
}

const std::vector<PluginObject *>&
View3D::getPlugins ()
{
  return m_plugins;
}

// View3DRotation

View3DRotation::View3DRotation (QWorkspace *parent)
  :View3D (parent, "OpenGL 3D View")
{
  m_glwidget = new OpenglWidget (this, "OpenGL 3D View", false, true);
  setCentralWidget (m_glwidget);
  setMouseTracking (true);
  m_editable = false;
}


void
View3DRotation::setupView ()
{
  glLoadIdentity ();

  gluLookAt (0.0, 0.0, 5.0, 
	     0.0, 0.0, 0.0, 
	     0.0, 1.0, 0.0);
}

void
View3DRotation::updateStatusBar (int x, int y)
{
}

void
View3DRotation::parseMousePress (QMouseEvent *e)
{
  
}

void
View3DRotation::parseMouseMove (QMouseEvent *e)
{
}

void
View3DRotation::parseMouseRelease (QMouseEvent *e)
{
  
}

