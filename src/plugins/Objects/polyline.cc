#include "plugin.h"
#include "../../math/vector3.h"
#include <qevent.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

#include "polyline.h"

#define PLUGIN_NAME "PolyLine Plugin"
#define PLUGIN_MENU "object/polylines"
#define PLUGIN_ICON "plugins/Objects/polyline.png"




PolyLine::PolyLine ()
  :PluginObject (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON), selected(0)
{
}

PolyLine::PolyLine (const std::string& name, const std::string& menu,  
		    const std::string& filename) :PluginObject (name, menu, filename)
{
}

void
PolyLine::buttonDown (QMouseEvent::ButtonState button, double x, double y, double z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    selected = getPoint (x, y, z);    
    if (selected)
      std::cout << PLUGIN_NAME " : dragging point " << *selected << std::endl;
    break;
  default:
    break;
  }
}

bool
PolyLine::buttonUp (QMouseEvent::ButtonState button, double x, double y, double z)
{
  switch (button) {
  case (QMouseEvent::LeftButton):
    if (!selected) {
      std::cout << PLUGIN_NAME " : adding point "<< x << " " << y << " " 
		<< z << std::endl;
      pts.push_back (new Vec3d(x,y,z));
      return false;
    } else {
      std::cout << PLUGIN_NAME " : dropping point to " << x << ", " << y
		<< ", " << z << std::endl;
      selected->setValues (x, y, z);
      selected = 0;
      return false;
    }
    break;    
  case (QMouseEvent::RightButton):
    std::cout << PLUGIN_NAME "try to remove " << x << ", " << y << ", " << z << std::endl;
    removePoint (x, y, z);
  default:
    break;
  }

  return true;
}

void
PolyLine::mouseMove (double x, double y, double z) 
{
  if (selected)
    selected->setValues (x, y, z);
}

bool
PolyLine::doubleClick (QMouseEvent::ButtonState button, double x, double y, double z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    std::cout << "adding point "<< x << " " << y << " " << z << std::endl;
    pts.push_back (new Vec3d(x,y,z));
    pts.push_back (new Vec3d(**(pts.begin())));
    endObject ();
    return true;
  }
  return false;
}


void
PolyLine::endObject ()
{  
}


bool
PolyLine::hasPoint (double x, double y, double z)
{
  if (getPoint (x, y, z))
    return true;
  return false;
}

Vec3d *
PolyLine::getPoint (double x, double y, double z) {
  std::list<Vec3d *>::iterator i = getPointIterator (x, y, z);
  if (i == pts.end ())
    return NULL;
  return *i;
}


std::list<Vec3d *>::iterator
PolyLine::getPointIterator (double x, double y, double z)
{
  double mindist = std::numeric_limits<double>::max();
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator min;
  std::list<Vec3d *>::iterator end = pts.end ();
  //  double epsilon = std::numeric_limits<typeof((*i)->x)>::epsilon();
  
  for (i=pts.begin(); i!=end; ++i) {
    double dist = hypot ((*i)->x-x, (*i)->y-y);
    if (dist < mindist) {
      mindist = dist;
      min = i;
    }
  }

  if (mindist < 0.075)
    return min;
  else
    return end;
}


void
PolyLine::removePoint (double x, double y, double z)
{
  Vec3d tmp=Vec3d(x, y, z);
  std::list<Vec3d *>::iterator i = getPointIterator (x, y, z);
  if (i != pts.end ()) {
    std::cout << PLUGIN_NAME " : removing " << tmp << std::endl;
    pts.erase (i);
  }
}

PolyLine::~PolyLine ()
{
}

void
PolyLine::drawPoints ()
{
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();
  glPushAttrib (GL_CURRENT_BIT);
  glPointSize (4.0f);
  glColor3f (1.0, 0.0, 0.0);
  glBegin (GL_POINTS);
  
  for (i=pts.begin(); i!=end; ++i)
    glVertex2f ((*i)->x, (*i)->y);

  glEnd ();
  glPopAttrib ();
}

void
PolyLine::display ()
{
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();

  glBegin (GL_LINE_STRIP);
  for (i=pts.begin(); i!=end; ++i) {
    glVertex2f ((*i)->x, (*i)->y); 
  }
  glEnd ();
  drawPoints ();
}

DECLARE_PLUGIN (PolyLine);
