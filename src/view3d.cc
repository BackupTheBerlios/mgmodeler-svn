#include "view3d.h"
#include "openglwidget.h"
#include "trackball.h"

#include <qgl.h>
#include <qstring.h>
#include <iostream>


View3D::View3D (QWorkspace *parent, std::string name)
  :QMainWindow (parent, name.c_str (), 0)
{
}


View3D::View3D (QWorkspace *parent, eAxe invaxe)
  : QMainWindow (parent, "OpenGL 2D View", 0)
{
  m_glwidget = new OpenglWidget (this, "OpenGL 2D View", true, false);
  setCentralWidget (m_glwidget);
  m_invaxe = invaxe;
  m_cursor_x = m_cursor_y=0;
  m_editable = true;
  setMouseTracking (true);
}

void
View3D::setupView ()
{
  double distance = 3.0;

  glLoadIdentity ();
  
  switch (m_invaxe)
    {
    case AXE_X:
      setCaption ("OpenGL 2D View: YZ");
      gluLookAt (-distance, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
      break;
    case AXE_Y:
      setCaption ("OpenGL 2D View: XZ");
      gluLookAt (0.0, -distance, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
      break;
    case AXE_Z:
      setCaption ("OpenGL 2D View: XY");
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
