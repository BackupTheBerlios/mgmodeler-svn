#include "view2d.h"
#include "plugin.h"
#include "gc.h"
#include "mainwindow.h"

#include <qgl.h>
#include <qstring.h>
#include <iostream>

PluginObject *View2D::s_plugin_current = NULL;
extern GeneralizedCylinder gc;
View2D::View2D (QWorkspace *parent, eView view,  MainWindow *wparent)
  : View (parent, "OpenGL 2D View")
{
  m_view = view;
  m_win_parent = wparent;
  m_cursor_x = m_cursor_y=0;
  setMouseTracking (true);
  
  m_plugin_active = NULL;

  m_mode = MODE_MOVE_WINDOW;
  m_mouse_move = false;
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
View2D::addPluginObject (PluginObject *obj)
{
  std::vector<PluginObject *>::iterator i;

  if (!obj)
    return;
  
  for (i = m_plugins.begin (); i!=m_plugins.end (); ++i)
    if ((*i) == obj)
	return;
  m_plugins.push_back (obj);
}

void
View2D::parseMousePress (QMouseEvent *e)
{
  if (e->button () == QMouseEvent::LeftButton)
    if (s_plugin_current)
      {
	if (m_plugin_active)
	  addPluginObject (m_plugin_active);
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
  
  if (e->button () == QMouseEvent::LeftButton)
    if (m_mode == MODE_MOVE_WINDOW)
      {
	beginWindowMotion (e->x (), e->y ());
      }

  if (m_mode == MODE_EDIT)
    if (m_plugin_active)
      {
	Vec3f v;
	OpenglWidget::unProject (Vec3f(e->x(), e->y(), 0), v);
	
	m_plugin_active->buttonDown (e->button (), e->state (), v);
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

  if (e->button () == QMouseEvent::LeftButton)
    if (m_mode == MODE_MOVE_WINDOW)
      {
	endWindowMotion ();
      }
  
  if (e->button () == QMouseEvent::LeftButton)
    if (m_mode == MODE_SELECTION_OBJECT && m_view!=VIEW_PROFIL)
      {
	std::vector<PluginObject *>::iterator i;
	std::vector<PluginObject *>::iterator end=m_plugins.end();
	Vec3f v;
	OpenglWidget::unProject (Vec3f(e->x(), e->y(), 0), v);

	for (i = m_plugins.begin (); i!=end; ++i)
	  {
	    if ((*i)->hasPoint (v))
	      {
		PluginObject *obj = m_plugin_active;
		m_plugin_active = (*i);
		addPluginObject (obj);
		m_win_parent->setViewsMode (MODE_EDIT);
		return;
	      }
	  }
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

      if (m_mode == MODE_MOVE_WINDOW)
	{
	  parseWindowMotion (e->x (), e->y());
	}
    
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
  
  glPushAttrib (GL_ENABLE_BIT);
  glDisable (GL_DEPTH_TEST);

  for (i = m_plugins.begin (); i!=end; ++i)
    if ((*i)!=m_plugin_active)
      (*i)->display ();

  if (m_plugin_active) {
    glColor3f(0, 0, 0);
    m_plugin_active -> display ();
    m_plugin_active -> drawNormals ();
  }

  glPopAttrib ();
}

void
View2D::updateStatusBar (const Vec3f& v)
{
  statusBar() ->clear ();
  
  statusBar() ->message (QString().setNum (v.x)+QString(" ")+
                         QString().setNum (v.y));
}

void 
View2D::beginWindowMotion (int x, int y)
{
  m_mouse_move = true;
  m_mouse_move_x = x;
  m_mouse_move_y = y;
}

void 
View2D::parseWindowMotion (int x, int y)
{
  int relx, rely;

  if (!m_mouse_move)
    return;

  relx = x - m_mouse_move_x;
  rely = m_mouse_move_y - y;

  m_mouse_move_x = x;
  m_mouse_move_y = y;
  
  getOpenglWidget ()-> moveWindow (relx, rely);
}

void 
View2D::endWindowMotion ()
{
  m_mouse_move = false;
}

void
View2D::load (std::istream& stream)
{
  int size;
  m_plugins.clear ();
  stream >> size;
  std::cout << "+" << size << std::endl;
  while (size--) {
    std::string name;
    stream >> name;
    std::cout << "-" << name << std::endl;
    Plugin *p = PluginManager::getPluginByName (name);
    if (p) {
      PluginObject *o=(PluginObject *)p->m_createinstance();
      o->load(stream);
      m_plugins.push_back (o);
    }
  }
}

void
View2D::save (std::ostream& stream) const
{
  std::vector<PluginObject *>::const_iterator i;
  std::vector<PluginObject *>::const_iterator end=m_plugins.end ();
  int size = m_plugins.size ();
  if (m_plugin_active)
    size ++;
  stream << size  << std::endl;
  
  for (i=m_plugins.begin(); i!=end; ++i) {
    stream << (*i)->getName() << std::endl;
    (*i)->save (stream);
  }
  if (m_plugin_active) {
    stream << m_plugin_active->getName() << std::endl;
    m_plugin_active->save (stream);
  }
}
