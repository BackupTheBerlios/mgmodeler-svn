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
  :PluginObject (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON), selected(0), should_close (false)
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
  case QMouseEvent::MidButton:
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
  case QMouseEvent::LeftButton:
    if (!selected) {
      std::cout << PLUGIN_NAME " : adding point "<< x << " " << y << " " 
		<< z << std::endl;
      if (!should_close)
	pts.push_back (new Vec3d(x,y,z));
      else
	pts.push_back (new Vec3d(**pts.begin()));
    }
    break;
  case QMouseEvent::ShiftButton:
    std::cout << "with shift pressed" << std::endl;
    break;
  case QMouseEvent::MidButton:
    if (selected) {
      std::cout << PLUGIN_NAME " : dropping point to " << x << ", " << y
		<< ", " << z << std::endl;
      selected->setValues (x, y, z);
      selected = 0;
    }

    break;
  case QMouseEvent::RightButton:
    std::cout << PLUGIN_NAME "try to remove " << x << ", " << y << ", " << z << std::endl;
    removePoint (x, y, z);
  default:
    break;
  }
  should_close=false;
  return true;
}

void
PolyLine::mouseMove (double x, double y, double z) 
{
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();
  if (selected)
    selected->setValues (x, y, z);
  cursor.setValues (x, y, z);
}

bool
PolyLine::doubleClick (QMouseEvent::ButtonState button, double x, double y, double z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    std::cout << PLUGIN_NAME " : closing" << std::endl;
    should_close = true;
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
PolyLine::drawPoints (std::list<Vec3d *>::iterator nearest)
{
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();
  glPushAttrib (GL_CURRENT_BIT);
  glPointSize (4.0f);
  glColor3f (1.0, 0.0, 0.0);
  glBegin (GL_POINTS);
  
  for (i=pts.begin(); i!=end; ++i) {
    if (i == nearest) {
      glColor3f (1.0, 1.0, 0.0);
      glVertex2f ((*i)->x, (*i)->y);
      ++i;
      glVertex2f ((*i)->x, (*i)->y);
      glColor3f (1.0, 0.0, 0.0);
    } else
      glVertex2f ((*i)->x, (*i)->y);
  }

  glEnd ();
  glPopAttrib ();

  for (i=pts.begin(); i!=end;) {
    std::list<Vec3d *>::iterator first = i++;
    if (i != end)
      distanceToSegment (Vec3d ((*i)->x, (*i)->y, 0),
			 **first, **i);
  }

}

void
PolyLine::display ()
{
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();
  std::list<Vec3d *>::iterator nearest;
  double mindist = std::numeric_limits<double>::max ();

  glBegin (GL_LINE_STRIP);
  for (i=pts.begin(); i!=end;) {
    std::list<Vec3d *>::iterator first = i++;
    glVertex2f ((*first)->x, (*first)->y); 
    if (i != end) {
      double dist = distanceToSegment (Vec3d (cursor.x, cursor.y, 0),
				       **first, **i);
      if (dist < mindist) {
	mindist = dist;
	nearest = first;
      }
    }
  }
  glEnd ();

  drawPoints (nearest);
}

double
PolyLine::distanceToSegment (const Vec3d& p, const Vec3d& f, const Vec3d& g) 
{
  double a, b;
  double c, d;
  double dx = (g.x - f.x);
  double dy = (g.y - f.y);
  double x, y;

  a =  dy / dx ;
  b = dy - a * dx;

  c = -1/a;
  d = p.y - c * p.x;

  x = (d - b) / (a - c);
  y = a * x + b;

  return hypot (x, y);
}


DECLARE_PLUGIN (PolyLine);
