#include "plugin.h"
#include "../../math/vector3.h"
#include <qevent.h>
#include <iostream>
#include <GL/gl.h>

#define PLUGIN_NAME "PolyLine Plugin"
#define PLUGIN_MENU "object/polylines"
#define PLUGIN_ICON "plugins/Objects/polyline.png"




class PolyLine : public PluginObject
{
public:
  PolyLine ();
  ~PolyLine ();
  void buttonDown (QMouseEvent::ButtonState button, double, double, double);
  bool buttonUp (QMouseEvent::ButtonState button, double, double, double);
  void endObject ();
  bool doubleClick (QMouseEvent::ButtonState  button, double, double, double);
  bool hasPoint (double, double, double);
  void removePoint (double, double, double);
  void display ();
private:
  std::list<Vec3d *> pts;
};

PolyLine::PolyLine ()
  :PluginObject (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON)
{
}

void
PolyLine::buttonDown (QMouseEvent::ButtonState button, double x, double y, double z)
{
}

bool
PolyLine::buttonUp (QMouseEvent::ButtonState button, double x, double y, double z)
{
  std::cout << "adding point "<< x << " " << y << " " << z << std::endl;
  pts.push_back (new Vec3d(x,y,z));
  return true;
}


bool
PolyLine::doubleClick (QMouseEvent::ButtonState button, double x, double y, double z)
{
  switch (button) {
  case QMouseEvent::LeftButton:
    std::cout << "adding point "<< x << " " << y << " " << z << std::endl;
    pts.push_back (new Vec3d(x,y,z));
    //    pts.push_back (new Vec3d(*pts[0]));
    endObject ();
    return true;
  case QMouseEvent::RightButton:
    removePoint (x, y, z);
    }
  return false;
}


void
PolyLine::endObject ()
{  
}


bool
PolyLine::hasPoint (double, double, double)
{
}


void
PolyLine::removePoint (double x, double y, double z)
{
  Vec3d tmp=Vec3d(x, y, z);
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();
  for (i=pts.begin(); i!=end; ++i)
    if (tmp==**i)
      pts.erase (i);
}

PolyLine::~PolyLine ()
{
}

void
PolyLine::display ()
{
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();
  glBegin (GL_LINE);
  for (i=pts.begin(); i!=end; ++i)
    glVertex2f ((*i)->x, (*i)->y);
  glEnd ();
  
}

DECLARE_PLUGIN (PolyLine);
