#include "view3d.h"
#include "openglwidget.h"
#include "trackball.h"
#include "plugin.h"

#include <qgl.h>
#include <qstring.h>
#include <iostream>

View3D::View3D (QWorkspace *parent, const std::string& name)
  : View (parent, name), current(0), normale_scale(1)
{
  setMouseTracking (true);
  m_glwidget->setOrthoView (false);
  m_glwidget->setTrackBall (true);
  setCaption (name);
}

void
View3D::setupView ()
{
  glLoadIdentity ();  
  gluLookAt (0.0, 0.0, 5.0, 
	     0.0, 0.0, 0.0, 
	     0.0, 1.0, 0.0);
}

void
View3D::parseMousePress (QMouseEvent *e)
{
}

void
View3D::parseMouseRelease (QMouseEvent *e)
{
}

void
View3D::parseMouseMove (QMouseEvent *e)
{  
}

void
View3D::parseMouseDoubleClick (QMouseEvent *e)
{
}


void
View3D::redisplay ()
{
  std::vector<Face> f;
  if (current)
    m_glwidget->drawPolygons (current->getPoints(),
			      current->getFaces ());
  else {
    f = gc.getFaces ();
    //gc.compute (f);
    if (f. size ())
      m_glwidget->drawPolygons (f);
  }
}


void 
View3D::setLighting ()
{
  m_glwidget->setLighting ();
}

void 
View3D::setFillMode ()
{
  m_glwidget->setWireframe (false);
}

void 
View3D::setWireframe ()
{
  m_glwidget->setWireframe (true);
}

void
View3D::toggleNormals ()
{
  m_glwidget->toggleNormals();
}

void
View3D::switchNormals ()
{
  normale_scale = -normale_scale;
  gc.setNormalScale(normale_scale);
}
