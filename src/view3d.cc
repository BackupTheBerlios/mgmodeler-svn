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
      gluLookAt (0.0, 0.0, distance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
      break;
    case VIEW_SECTION:
      setCaption ("OpenGL 2D View: Section");
      gluLookAt (0.0, 0.0, distance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
      break;
    case VIEW_WAY:
      setCaption ("OpenGL 2D View: Way");
      gluLookAt (0.0, 0.0, distance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);      
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


void
View3D::parseMousePress (QMouseEvent *e)
{
  if (e->button () == QMouseEvent::LeftButton)
    if (s_plugin_current)
      {
	//TODO push old
	m_plugin_active = (PluginObject *)
	  s_plugin_current-> m_createinstance ();
	s_plugin_current = NULL;
	//std::cout<<"NEW PLUGIN IN EDIT MODE\n";
      }

  if (m_plugin_active)
    m_plugin_active->buttonDown (e->button(), e->x(), e->y (), 0);
}

void
View3D::parseMouseMove (QMouseEvent *e)
{
  if (e->x ()>0 && e->y ()>0)
    {
      m_cursor_x = e-> x();
      m_cursor_y = e-> y();
      updateStatusBar (m_cursor_x, m_cursor_y);
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
