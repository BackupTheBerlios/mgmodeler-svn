#include "plugin.h"
#include "../../math/vector3.h"
#include <qevent.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

#define PLUGIN_NAME "Circle Plugin"
#define PLUGIN_MENU "Circles"
#define PLUGIN_ICON "plugins/Objects/circle.png"

class Circle : public PluginObject {
public:
  Circle ();
  ~Circle ();
  void buttonDown (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, const Vec3f& v);
  bool buttonUp (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, const Vec3f& v);
  void endObject ();
  bool doubleClick (QMouseEvent::ButtonState  button, QMouseEvent::ButtonState state, const Vec3f& v);
  bool hasPoint (const Vec3f& v);
  void removePoint (const Vec3f& v);
  void display () const;
  void drawPoints () const;
  void mouseMove (QMouseEvent::ButtonState state, const Vec3f& v);
  float evaluate (std::vector<Vec3f>& res) const;
  void evaluateTimeline (std::vector<float>& time) const;
  void evaluateNormals (std::vector<Vec3f>& normals) const;
  void drawNormals () const;
private:
  Vec3f center;
  Vec3f radius;
  float r;
  bool drawing;
};



Circle::Circle ()
  :PluginObject (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON), drawing(false),
   r(1.0)
{  
}

void
Circle::buttonDown (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, const Vec3f& v)
{
  center = radius = v;
  drawing = true;
  std::cout << PLUGIN_NAME" : setting center to " << center << std::endl;
}

bool
Circle::buttonUp (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, const Vec3f& v)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    radius = v;
    r = (radius - center).length ();
    std::cout << PLUGIN_NAME" : setting radius control point to " << radius 
	      << std::endl;
    drawing = false;
    return false;
    break;
    
  case QMouseEvent::RightButton:
    removePoint (v);
  default:
    std::cout << "blabla" << std::endl;
  }

  return true;
}


bool
Circle::doubleClick (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, const Vec3f& v)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    return true;
  }
  return false;
}

void
Circle::mouseMove (QMouseEvent::ButtonState state, const Vec3f& v)
{
  if (drawing)
    radius = v;
}


void
Circle::endObject ()
{  
}


bool
Circle::hasPoint (const Vec3f& v)
{
  float epsilon = std::numeric_limits<typeof(center.x)>::epsilon();
  
  if (std::abs(center.x-v.x) < epsilon && 
      std::abs(center.y-v.y) < epsilon)
    return true;
  if (std::abs(radius.x-v.x) < epsilon && 
      std::abs(radius.y-v.y) < epsilon)
    return true;
  return false;
}


void
Circle::removePoint (const Vec3f& v)
{
}

Circle::~Circle ()
{
}


void
Circle::drawPoints () const
{
  glPushAttrib (GL_CURRENT_BIT);
  glPointSize (4.0f);
  glColor3f (1.0, 0.0, 0.0);
  glBegin (GL_POINTS);
  glVertex2f (center.x, center.y);
  glVertex2f (radius.x, radius.y);
  glEnd ();
  glPopAttrib ();
}

void
Circle::display () const
{
  double r = hypot (radius.x - center.x, radius.y - center.y);
  double astep = M_PI/16.;
  double a = 0;
  glBegin (GL_LINE_STRIP);
  for (int i=0; i <= 32; i++) {
    glVertex2f (center.x + r * cos (a),
		center.y + r * sin (a));
    a += astep;
  }
  glEnd ();
  drawPoints ();
}

float
Circle::evaluate (std::vector<Vec3f>& res) const
{
  
  res.clear ();
  assert (resolution);
  double astep = M_PI/(resolution/2.);
  double a = 0;
  for (int i=0; i <= resolution; i++) {
    res.push_back (Vec3f (center.x + r * cos (a),
			  center.y + r * sin (a), 0));
    a += astep;
  }
  return r*2*M_PI;
}

void
Circle::evaluateTimeline (std::vector<float>& timeline) const
{
  timeline.clear ();
  assert (resolution);

  float timestep = 1.0f / resolution;
  float t = 0;

  for (int i=0; i <= resolution; i++) {
    timeline.push_back (t);
    t+=timestep;
  }
  
}

void
Circle::evaluateNormals (std::vector<Vec3f>& normals) const
{
  normals.clear ();
  assert (resolution);

  double astep = M_PI/(resolution/2.);
  double a = 0;
  for (int i=0; i <= resolution; i++) {
    Vec3f v(/*center.x +*/ r * cos (a), /*center.y + */r * sin (a), 0);
    Vec3f n(cos(a), sin(a), 0);
    normals.push_back (n);
    a += astep;
  }
}

void
Circle::drawNormals () const
{
  int c;
  std::vector<Vec3f> pts;
  evaluate(pts);
  std::vector<Vec3f>::iterator i;
  std::vector<Vec3f>::iterator end = pts.end ();
  std::vector<Vec3f> normals;
  evaluateNormals (normals);
  if (normals.empty())
    return;
  glBegin (GL_LINES);
  for (i=pts.begin(), c= 0 ; i!=end; ++i, c++) {
    Vec3f white(1, 1, 1);
    Vec3f blue(0, 0, 1);
    Vec3f p=(*i);
    glColor3fv(&white[0]);
    glVertex3fv(&p[0]);
    glColor3fv(&blue[0]);
    glVertex3fv(&(p+normals[c]/10)[0]);
  }
  glEnd ();
}

DECLARE_PLUGIN (Circle);
