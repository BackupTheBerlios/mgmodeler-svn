#include "view3d.h"
#include "openglwidget.h"
#include "trackball.h"
#include "plugin.h"
#include "gc.h"

#include <qgl.h>
#include <qstring.h>
#include <iostream>

GeneralizedCylinder gc;

PluginObject *View3D::s_plugin_current = NULL;

View3D::View3D (QWorkspace *parent, std::string name)
  :QMainWindow (parent, name.c_str (), 0)
{
  m_plugin_active = NULL;
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
View3D::updateStatusBar (float x, float y)
{
  statusBar() ->clear ();
  
  statusBar() ->message (QString().setNum (x)+QString(" ")+
			 QString().setNum (y));
  
}


static void
unProject (int x, int y, double *dx, double *dy, double *dz)
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
  *dy=-*dy;
}

void
View3D::parseMousePress (QMouseEvent *e)
{
  if (e->button () == QMouseEvent::LeftButton)
    if (s_plugin_current)
      {
	if (m_plugin_active)
	  m_plugins.push_back (m_plugin_active);
	m_plugin_active = (PluginObject *)
	  s_plugin_current-> m_createinstance ();
	switch (m_view) {
	case VIEW_PROFIL:
	  gc.addProfile (m_plugin_active);
	  break;
	case VIEW_SECTION:
	  gc.addSection (m_plugin_active);
	  break;
	case VIEW_WAY:
	  gc.addPath (m_plugin_active);
	  break;
	} 
	s_plugin_current = NULL;
      }

  if (m_mode == MODE_EDIT)
    if (m_plugin_active)
      {
	double x, y, z;
	unProject (e->x(), e->y(), &x ,&y, &z);
	
	m_plugin_active->buttonDown (e->button (), e->state (), x, y, z);
      }
  if (m_mode == MODE_SELECTION)
    {
      //      PluginObject *objp = findObject (e-> x(), e-> y());
    }

}

void
View3D::parseMouseRelease (QMouseEvent *e)
{
  if (m_mode == MODE_EDIT)
    {
      double x, y, z;
      unProject (e->x(), e->y(), &x ,&y, &z);
      
      if (m_plugin_active)
	m_plugin_active->buttonUp (e->button (), e->state (), x, y, z);
    }
}

void
View3D::parseMouseMove (QMouseEvent *e)
{
  if (e->x ()>0 && e->y ()>0)
    {
      m_cursor_x = e-> x();
      m_cursor_y = e-> y();
      
      double x, y, z;
      unProject (e->x(), e->y(), &x ,&y, &z);
  
      if (m_mode == MODE_EDIT)
	if (m_plugin_active)
	  m_plugin_active->mouseMove (e-> state (), x, y, z);

	;
    
      updateStatusBar (x, y);
    }
  
}

void
View3D::parseMouseDoubleClick (QMouseEvent *e)
{
  if (m_mode == MODE_EDIT)
    {
      double x, y, z;
      unProject (e->x(), e->y(), &x ,&y, &z);
      if (m_plugin_active)
	m_plugin_active->doubleClick (e->button(), e->state (), x, y, z);
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
  :View3D (parent, "OpenGL 3D View"), current(0)
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
View3DRotation::updateStatusBar (float x, float y)
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


void 
View3DRotation::drawPolygons (const std::vector<std::vector<Vec3f> >& faces)
{
  std::vector<std::vector<Vec3f> >::const_iterator i;
  std::vector<Vec3f>::const_iterator j;
  //  std::cout << "Drawing polygons ";
  int current = 0;
  for (i=faces.begin (); i!= faces.end (); i++, current++)
    {
      //  std::cout << "!";
      glBegin (GL_POLYGON);
      switch (current % 3) {
      case 0:
	glColor3f (1.f, 0.f, 0.f);
	break;
      case 1:
	glColor3f (0.f, 1.f, 0.f);
	break;
      case 2:
	glColor3f (0.f, 0.f, 1.f);
	break;
      }
      for (j = (*i).begin (); j!=(*i).end (); j++)
	{
	  
	  glVertex3f ((*j).x, (*j).y, (*j).z);
	  
	}
      glEnd ();
    }
 
}

void 
View3DRotation::drawPolygons (const std::vector<Vec3f>& points,
			      const std::vector<std::vector<int> >& faces)
{
  std::vector<std::vector<int> >::const_iterator i;
  std::vector<int>::const_iterator j;

  glPushAttrib(~0);
  glDisable(GL_LIGHTING); 
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); 
  //glEnable(GL_POLYGON_OFFSET_LINE);

  int current = 0;
  for (i=faces.begin (); i!= faces.end (); i++, current++)
    {
      glBegin (GL_POLYGON);
      switch (current % 3) {
      case 0:
	glColor3f (1.f, 0.f, 0.f);
	break;
      case 1:
	glColor3f (0.f, 1.f, 0.f);
	break;
      case 2:
	glColor3f (0.f, 0.f, 1.f);
	break;
      }
      for (j = (*i).begin (); j!=(*i).end (); j++)
	{
	  glVertex3f (points[(*j)].x, points[(*j)].y, points[(*j)].z);
	  
	}
      glEnd ();
    }
  glPopAttrib();
}

