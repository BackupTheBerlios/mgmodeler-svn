#include "plugin.h"
#include "math/vector3.h"
#include <qevent.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

#include "polyline.h"

#define PLUGIN_NAME "PolyLine"
#define PLUGIN_MENU "Polylines"
#define PLUGIN_ICON "polyline.png"


PolyLine::PolyLine ()
  : PluginObject (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON), selected(0),  
    need_recompute(true), should_close (false)
{
}

PolyLine::PolyLine (const std::string& name, const std::string& menu,  
		    const std::string& filename) 
  : PluginObject (name, menu, filename), selected(0), 
    need_recompute(true), should_close(false)
{
}

void
PolyLine::buttonDown (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, const Vec3f& v)
{
  switch (button) {
  case QMouseEvent::MidButton:
    selected = getPoint (v);
    need_recompute = false;
    if (selected)
      need_recompute = true;
    break;
  default:
    break;
  }
}

bool
PolyLine::buttonUp (QMouseEvent::ButtonState button, 
		    QMouseEvent::ButtonState state,const Vec3f& v)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    if (!(int)(state & QMouseEvent::ShiftButton)) {
      if (!should_close)
	pts.push_back (new Vec3f(v));
      else
	pts.push_back (new Vec3f(**pts.begin()));
    } else {
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
	pts.insert (++nearest, new Vec3f (v));
    }
    break;
  case QMouseEvent::MidButton:
    if (selected) {
      selected->setValues (v.x, v.y, v.z);
      selected = 0;
    }

    break;
  case QMouseEvent::RightButton:
    removePoint (v);
  default:
    break;
  }
  should_close=false;
  need_recompute = true;
  return true;
}

void
PolyLine::mouseMove (QMouseEvent::ButtonState state,const Vec3f& v) 
{
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator end = pts.end ();
  if (selected)
    selected->setValues (v.x, v.y, v.z);
  cursor.setValues (v.x, v.y, v.z);
}

bool
PolyLine::doubleClick (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state,const Vec3f& v)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    should_close = true;
    return true;
  default:
    break;
  }
  return false;
}


void
PolyLine::endObject ()
{  
}


bool
PolyLine::hasPoint (const Vec3f& v)
{
  if (getPoint (v))
    return true;
  return false;
}

Vec3f *
PolyLine::getPoint (const Vec3f& v) {
  std::list<Vec3f *>::iterator i = getPointIterator (v);
  if (i == pts.end ())
    return NULL;
  return *i;
}


std::list<Vec3f *>::iterator
PolyLine::getPointIterator (const Vec3f& v)
{
  float mindist = std::numeric_limits<float>::max();
  std::list<Vec3f *>::iterator i;
  std::list<Vec3f *>::iterator min;
  std::list<Vec3f *>::iterator end = pts.end ();
  
  for (i=pts.begin(); i!=end; ++i) {
    float dist = hypot ((*i)->x-v.x, (*i)->y-v.y);
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
PolyLine::removePoint (const Vec3f& v)
{
  std::list<Vec3f *>::iterator i = getPointIterator (v);
  if (i != pts.end ())
    pts.erase (i);
  need_recompute = true;
}

PolyLine::~PolyLine ()
{
}

void
PolyLine::drawPoints (const std::list<Vec3f *>::const_iterator& nearest) const
{
  std::list<Vec3f *>::const_iterator i;
  std::list<Vec3f *>::const_iterator end = pts.end ();
  glPushAttrib (GL_CURRENT_BIT);
  glPointSize (4.0f);
  glColor3f (1.0, 0.0, 0.0);
  glBegin (GL_POINTS);
  
  for (i=pts.begin(); i!=end; ++i) {
    if (i == nearest) {
      glColor3f (1.0, 0.0, 0.0);
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
PolyLine::drawNormals () const
{
  int c;
  std::list<Vec3f *>::const_iterator i;
  std::list<Vec3f *>::const_iterator end = pts.end ();

  std::vector<Vec3f> normals;

  evaluateNormals (normals);
  if (normals.empty())
    return;

  glBegin (GL_LINES);
  for (i=pts.begin(), c= 0 ; i!=end; ++i, c+=resolution) {
    Vec3f white(1, 1, 1);
    Vec3f blue(0, 0, 1);
    Vec3f p=*(*i);
    glColor3fv(&white[0]);
    glVertex3fv(&p[0]);
    glColor3fv(&blue[0]);
    glVertex3fv(&(p+normals[c]/10)[0]);
  }
  glEnd ();
}

void
PolyLine::display () const
{
  std::list<Vec3f *>::const_iterator i;
  std::list<Vec3f *>::const_iterator end = pts.end ();
  std::list<Vec3f *>::const_iterator nearest;
  float mindist = std::numeric_limits<float>::max ();


  i = pts.begin ();
  if (i == end)
    return;

  glBegin (GL_LINE_STRIP);
  while (i!=end) {
    std::list<Vec3f *>::const_iterator first = i++;
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
PolyLine::distanceToSegment (const Vec3f& p, const Vec3f& f, const Vec3f& g) const 
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

float
PolyLine::evaluate (std::vector<Vec3f>& res) const
{
  std::list<Vec3f *>::const_iterator i;
  std::list<Vec3f *>::const_iterator end;
  float length = 0;

  res.clear ();
  if (pts.empty ())
    return 0;

  end = pts.end ();
  i = pts.begin ();

  while (i != end) {
    std::list<Vec3f *>::const_iterator first = i++;
    if (i != end) {
      Vec3f u(**first);
      float dx = -((*first)->x-(*i)->x);
      float dy = -((*first)->y-(*i)->y);
      Vec3f v(dx, dy, 0);
      v/=resolution;
      for (int c=0; c<resolution; c++) {
	length += hypot (dx/resolution, dy/resolution);
	res.push_back (u);
	u+=v;
      }
    } else {
      res.push_back (**first);
    }
  }  
  return length;
}

void
PolyLine::evaluateTimeline (std::vector<float>& t) const
{
  float miny = std::numeric_limits<float>::max();
  float maxy = std::numeric_limits<float>::min();

  std::list<Vec3f *>::const_iterator i;
  std::list<Vec3f *>::const_iterator end;
  
  t.clear ();

  if (pts.empty ())
    return;

  end = pts.end ();
  i = pts.begin ();
  while (i != end) {
    float y = (*i)->y;
    std::list<Vec3f *>::const_iterator first = i++;
    if (miny>y)
      miny = y;
    if (maxy<y)
      maxy = y;
    t.push_back (y);
    if (i==end)
      break;
    float dy = ((*i)->y-(*first)->y);
    dy/=resolution;
    for (int c=0; c<resolution-1; c++) {
      y+=dy;
      t.push_back (y);
    }
  }
  for (unsigned int i=0;i<t.size (); i++)
    t[i] = (t[i] - miny)/ (maxy-miny);  
}

void
PolyLine::evaluateNormals (std::vector<Vec3f>& normals) const
{
  Vec3f z (0, 0, 1);
  std::list<Vec3f *>::const_iterator i;
  std::list<Vec3f *>::const_iterator end = pts.end ();

  normals.clear ();
  if (pts.size()<2)
    return;

  i = pts.begin ();

  while (i != end) {
    std::list<Vec3f *>::const_iterator first = i++;

    if (i != end) {
      float dx = ((*first)->x-(*i)->x);
      float dy = ((*first)->y-(*i)->y);
      Vec3f v (dx, dy, 0);
      Vec3f n = v.cross (z);
      for (int c=0; c<resolution; c++)
	normals.push_back (n);
    }
  }

  Vec3f last (normals[0]);
  for (unsigned int c=0; c<normals.size(); c++) {
    Vec3f tmp = last;
    last = normals[c];
    normals[c] = (normals[c]+tmp)/2;
    normals[c].normalize();
  }

  Vec3f nend(**(++pts.rbegin())-**(pts.rbegin()));
  nend[2]=0;
  Vec3f n = nend.cross (z);
  n.normalize ();
  normals.push_back (n);
}

void
PolyLine::load (std::istream &stream)
{
  int size;
  pts.clear ();
  stream >> size;
  for (int i=0; i<size; i++) {
    float x=0, y=0, z=0;
    stream >> x >> y >> z;
    std::cout << Vec3f(x, y, z) << std::endl;
    pts.push_back (new Vec3f (x, y, z));
  } 
}

void
PolyLine::save (std::ostream &stream) const
{
  std::list<Vec3f *>::const_iterator i;
  std::list<Vec3f *>::const_iterator end = pts.end ();
  int size = pts.size ();
  stream << size << std::endl;
  for (i=pts.begin (); i!=end; ++i)
    stream << (**i).x << " " << (**i).y << " " << (**i).z << std::endl;
}


DECLARE_PLUGIN (PolyLine);
