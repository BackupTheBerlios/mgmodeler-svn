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
  void buttonDown (QMouseEvent::ButtonState button, double, double, double);
  bool buttonUp (QMouseEvent::ButtonState button, double, double, double);
  void endObject ();
  bool doubleClick (QMouseEvent::ButtonState  button, double, double, double);
  bool hasPoint (double, double, double);
  void removePoint (double, double, double);
  void display ();
  void drawPoints ();
  void mouseMove (double x, double y, double z) {}
private:
  Vec3f center;
  Vec3f radius;
};



Circle::Circle ()
  :PluginObject (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON)
{  
}

void
Circle::buttonDown (QMouseEvent::ButtonState button, double x, double y, double z)
{
  center = Vec3f(x, y, z);
  std::cout << PLUGIN_NAME" : setting center to " << center << std::endl;
}

bool
Circle::buttonUp (QMouseEvent::ButtonState button, double x, double y, double z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    radius = Vec3f (x, y, z);
    std::cout << PLUGIN_NAME" : setting radius control point to " << radius 
	      << std::endl;
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
Circle::doubleClick (QMouseEvent::ButtonState button, double x, double y, double z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    return true;
  }
  return false;
}


void
Circle::endObject ()
{  
}


bool
Circle::hasPoint (double x, double y, double z)
{
  double epsilon = std::numeric_limits<typeof(center.x)>::epsilon();
  
  if (std::abs(center.x-x) < epsilon && 
      std::abs(center.y-y) < epsilon)
    return true;
  if (std::abs(radius.x-x) < epsilon && 
      std::abs(radius.y-y) < epsilon)
    return true;
  return false;
}


void
Circle::removePoint (double x, double y, double z)
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
  drawPoints ();
}

DECLARE_PLUGIN (Circle);
