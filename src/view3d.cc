#include "view3d.h"
#include "openglwidget.h"
#include "trackball.h"
#include "plugin.h"
#include "gc.h"

#include <qgl.h>
#include <qstring.h>
#include <iostream>

GeneralizedCylinder gc;

View3D::View3D (QWorkspace *parent, const std::string& name)
  : View (parent, name), current(0)
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
    gc.compute (f);
    m_glwidget->drawPolygons (f);
  }
}
