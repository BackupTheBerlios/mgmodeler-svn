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
		    const std::string& filename) :PluginObject (name, menu, filename), selected(0), should_close(false)
{
}

void
PolyLine::buttonDown (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, float x, float y, float z)
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
PolyLine::buttonUp (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state,float x, float y, float z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    std::cout << (int)(state & QMouseEvent::ShiftButton) << std::endl;
    if (!(int)(state & QMouseEvent::ShiftButton)) {
      std::cout << PLUGIN_NAME " : adding point "<< x << " " << y << ", " 
		<< z << std::endl;
      if (!should_close)
	pts.push_back (new Vec3f(x,y,z));
      else
	pts.push_back (new Vec3f(**pts.begin()));
    } else {
      std::cout << PLUGIN_NAME << " : inserting point" << x << ", " << y
		<< ", "	<< z << std::endl;
      std::list<Vec3f *>::iterator i;
      std::list<Vec3f *>::iterator end = pts.end ();
      std::list<Vec3f *>::iterator nearest = end;
      float mindist = std::numeric_limits<float>::max ();
      
      i = pts.begin ();
      if (i == end)
	break;
      
      while (i!=end) {
	std::list<Vec3f *>::iterator first = i++;
	if (first == end)
	  break;
	glVertex2f ((*first)->x, (*first)->y); 
	if (i != end) {
	  float dist = distanceToSegment (Vec3f (cursor.x, cursor.y, 0),
					  **first, **i);
	  if (dist < mindist) {
	    mindist = dist;
	    nearest = first;
	  }
	}
      }
      if (nearest != end)
	pts.insert (++nearest, new Vec3f (x, y ,z));
    }
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
PolyLine::mouseMove (QMouseEvent::ButtonState state,float x, float y, float z) 
{
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator end = pts.end ();
  if (selected)
    selected->setValues (x, y, z);
  cursor.setValues (x, y, z);
}

bool
PolyLine::doubleClick (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state,float x, float y, float z)
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
PolyLine::hasPoint (float x, float y, float z)
{
  if (getPoint (x, y, z))
    return true;
  return false;
}

Vec3f *
PolyLine::getPoint (float x, float y, float z) {
  std::list<Vec3f *>::iterator i = getPointIterator (x, y, z);
  if (i == pts.end ())
    return NULL;
  return *i;
}


std::list<Vec3f *>::iterator
PolyLine::getPointIterator (float x, float y, float z)
{
  float mindist = std::numeric_limits<float>::max();
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator min;
  std::list<Vec3f *>::iterator end = pts.end ();
  //  float epsilon = std::numeric_limits<typeof((*i)->x)>::epsilon();
  
  for (i=pts.begin(); i!=end; ++i) {
    float dist = hypot ((*i)->x-x, (*i)->y-y);
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
PolyLine::removePoint (float x, float y, float z)
{
  Vec3f tmp=Vec3f(x, y, z);
  std::list<Vec3f *>::iterator i = getPointIterator (x, y, z);
  if (i != pts.end ()) {
    std::cout << PLUGIN_NAME " : removing " << tmp << std::endl;
    pts.erase (i);
  }
}

PolyLine::~PolyLine ()
{
}

void
PolyLine::drawPoints (std::list<Vec3f *>::iterator nearest)
{
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator end = pts.end ();
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
}

void
PolyLine::display ()
{
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator end = pts.end ();
  std::list<Vec3f *>::iterator nearest;
  float mindist = std::numeric_limits<float>::max ();

  i = pts.begin ();
  if (i == end)
    return;

  glBegin (GL_LINE_STRIP);
  while (i!=end) {
    std::list<Vec3f *>::iterator first = i++;
    if (first == end)
      break;
    glVertex2f ((*first)->x, (*first)->y); 
    if (i != end) {
      float dist = distanceToSegment (Vec3f (cursor.x, cursor.y, 0),
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

float
PolyLine::distanceToSegment (const Vec3f& p, const Vec3f& f, const Vec3f& g) 
{
  float a, b;
  float c, d;
  float dx = (g.x - f.x);
  float dy = (g.y - f.y);
  float x, y;

  if (dx == 0 && dy == 0)
    return hypot (p.x - f.x, p.y - f.y);
  if (dx == 0)
    return distanceToSegment (Vec3f (p.y, p.x, 0), Vec3f (f.y, f.x, 0),
			      Vec3f (g.y, g.x, 0));

  // y=a*x+b
  a =  dy / dx ;
  b = g.y - a * g.x;

  // perpendicular y=-1/a*x+d
  c = -1./a;
  d = p.y - c * p.x;

  // intersection
  x = (d - b) / (a - c);
  y = a * x + b;

  // return hypot (p.x - x, p. y - y);
  return hypot (f.x - p.x, f.y - p .y) + hypot (g.x - p.x, g.y - p.y);
}

void
PolyLine::evaluate (std::vector<Vec3f>& res)
{
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator end;
  res.clear ();
  if (pts.empty ())
    return;

  end = pts.end ();

  for (i = pts.begin (); i != end; ++i)
    res.push_back (**i);
}

void
PolyLine::evaluateTimeline (std::vector<float>& t)
{
  float dist = 0.0f;
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator end;
  
  t.clear ();

  if (pts.empty ())
    return;

  i = pts.begin ();
  end = pts.end ();

  while (i != end) {
    std::list<Vec3f *>::iterator first = i++;
    t.push_back (dist);
    if (i != end) {
      float dx = ((*i)->x-(*first)->x);
      float dy = ((*i)->y-(*first)->y);
      float l = sqrt (dx * dx + dy * dy);
      dist += l;
    }
  }

  for (int i=0;i<t.size (); i++)
    t[i]/=dist;  
}

void
PolyLine::evaluateNormals (std::vector<Vec3f>& normals) 
{
  Vec3f z (0, 0, 1);
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator end = pts.end ();

  normals.clear ();

  i = pts.begin ();
  if (i == end)
    return;

  while (i != end) {
    std::list<Vec3f *>::iterator first = i++;

    if (i != end) {
      float dx = ((*i)->x-(*first)->x);
      float dy = ((*i)->y-(*first)->y);
      Vec3f v (dx, dy, 0);
      Vec3f n = v.cross (z);
      n.normalize ();
      normals.push_back (n);
    }
  }

}


DECLARE_PLUGIN (PolyLine);
