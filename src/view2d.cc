#include "view2d.h"
#include "plugin.h"
#include "gc.h"

#include <qgl.h>
#include <qstring.h>
#include <iostream>

PluginObject *View2D::s_plugin_current = NULL;
extern GeneralizedCylinder gc;
View2D::View2D (QWorkspace *parent, eView view)
  : View (parent, "OpenGL 2D View")
{
  m_view = view;
  m_cursor_x = m_cursor_y=0;
  setMouseTracking (true);
  
  m_plugin_active = NULL;

  m_mode = MODE_EDIT;
}

void
View2D::setupView ()
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
    case VIEW_PATH:
      setCaption ("OpenGL 2D View: Path");
      break;
    default:
      std::cout << "Warning Invalid Axe"<<std::endl;
      break;

    }
}

void
View2D::updateStatusBar (float x, float y)
{
  statusBar() ->clear ();
  
  statusBar() ->message (QString().setNum (x)+QString(" ")+
			 QString().setNum (y));
  
}



void
View2D::parseMousePress (QMouseEvent *e)
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
	case VIEW_PATH:
	  gc.addPath (m_plugin_active);
	  break;
	} 
	s_plugin_current = NULL;
      }

  if (m_mode == MODE_EDIT)
    if (m_plugin_active)
      {
	Vec3f v;
	OpenglWidget::unProject (Vec3f(e->x(), e->y(), 0), v);
	
	m_plugin_active->buttonDown (e->button (), e->state (), v);
      }
  if (m_mode == MODE_SELECTION)
    {
      //      PluginObject *objp = findObject (e-> x(), e-> y());
    }

}

void
View2D::parseMouseRelease (QMouseEvent *e)
{
  if (m_mode == MODE_EDIT)
    {
      Vec3f v;
      OpenglWidget::unProject (Vec3f(e->x(), e->y(), 0),
			       v);
      
      if (m_plugin_active)
	m_plugin_active->buttonUp (e->button (), e->state (), v);
    }
}

void
View2D::parseMouseMove (QMouseEvent *e)
{
  if (e->x ()>0 && e->y ()>0)
    {
      m_cursor_x = e-> x();
      m_cursor_y = e-> y();
      
      Vec3f v;
      OpenglWidget::unProject (Vec3f (e->x(), e->y(), 0), v);
  
      if (m_mode == MODE_EDIT)
	if (m_plugin_active)
	  m_plugin_active->mouseMove (e-> state (), v);

	;
    
      updateStatusBar (v);
    }
  
}

void
View2D::parseMouseDoubleClick (QMouseEvent *e)
{
  if (m_mode == MODE_EDIT)
    {
      Vec3f v;
      OpenglWidget::unProject (Vec3f (e->x(), e->y(), 0), v);
      if (m_plugin_active)
	m_plugin_active->doubleClick (e->button(), e->state (), v);
    }
}

void
View2D::setCurrentPlugin (PluginObject *p) 
{
  s_plugin_current = p;
}

const std::vector<PluginObject *>&
View2D::getPlugins ()
{
  return m_plugins;
}

void
View2D::redisplay ()
{
  std::vector<PluginObject *>::iterator i;
  std::vector<PluginObject *>::iterator end=m_plugins.end();
 
  for (i = m_plugins.begin (); i!=end; ++i)
    (*i)->display ();

  if (m_plugin_active) {
    m_plugin_active -> display ();
    m_plugin_active -> drawNormals ();
  }
}

void
View2D::updateStatusBar (const Vec3f& v)
{
  statusBar() ->clear ();
  
  statusBar() ->message (QString().setNum (v.x)+QString(" ")+
                         QString().setNum (v.y));
}
