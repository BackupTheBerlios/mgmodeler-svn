#include "plugin.h"
#include "../../math/vector3.h"
#include <qevent.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

#define PLUGIN_NAME "Circle Plugin"
#define PLUGIN_MENU "object/circles"
#define PLUGIN_ICON "plugins/Objects/circle.png"

class Circle : public PluginObject {
public:
  Circle ();
  ~Circle ();
  void buttonDown (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, float, float, float);
  bool buttonUp (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, float, float, float);
  void endObject ();
  bool doubleClick (QMouseEvent::ButtonState  button, QMouseEvent::ButtonState state, float, float, float);
  bool hasPoint (float, float, float);
  void removePoint (float, float, float);
  void display ();
  void drawPoints ();
  void mouseMove (QMouseEvent::ButtonState state, float x, float y, float z);
  void evaluate (int resolution, std::vector<Vec3f>& res);
private:
  Vec3f center;
  Vec3f radius;
  bool drawing;
};



Circle::Circle ()
  :PluginObject (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON), drawing(false)
{  
}

void
Circle::buttonDown (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, float x, float y, float z)
{
  center = radius = Vec3f(x, y, z);
  drawing = true;
  std::cout << PLUGIN_NAME" : setting center to " << center << std::endl;
}

bool
Circle::buttonUp (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, float x, float y, float z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    radius = Vec3f (x, y, z);
    std::cout << PLUGIN_NAME" : setting radius control point to " << radius 
	      << std::endl;
    drawing = false;
    return false;
    break;
    
  case QMouseEvent::RightButton:
    removePoint (x, y, z);
  default:
    std::cout << "blabla" << std::endl;
  }

  return true;
}


bool
Circle::doubleClick (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, float x, float y, float z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    return true;
  }
  return false;
}

void
Circle::mouseMove (QMouseEvent::ButtonState state, float x, float y, float z)
{
  if (drawing)
    radius = Vec3f (x, y,z);
}


void
Circle::endObject ()
{  
}


bool
Circle::hasPoint (float x, float y, float z)
{
  float epsilon = std::numeric_limits<typeof(center.x)>::epsilon();
  
  if (std::abs(center.x-x) < epsilon && 
      std::abs(center.y-y) < epsilon)
    return true;
  if (std::abs(radius.x-x) < epsilon && 
      std::abs(radius.y-y) < epsilon)
    return true;
  return false;
}


void
Circle::removePoint (float x, float y, float z)
{
}

Circle::~Circle ()
{
}


void
Circle::drawPoints ()
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
Circle::display ()
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

void
Circle::evaluate (int resolution, std::vector<Vec3f>& res)
{
  res.clear ();

  double r = hypot (radius.x - center.x, radius.y - center.y);
  double astep = M_PI/(resolution/2);;
  double a = 0;
  
  for (int i=0; i <= resolution; i++) {
    res.push_back (Vec3f (center.x + r * cos (a),
		center.y + r * sin (a), 0));
    a += astep;
  }
  
}

DECLARE_PLUGIN (Circle);
