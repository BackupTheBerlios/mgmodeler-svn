#include "plugin.h"
#include <iostream>

#define PLUGIN_NAME "PolyLine Plugin"
#define PLUGIN_MENU "object/polylines"
#define PLUGIN_ICON "icons/polyline.png"

struct point {
  point (const point &p) : x (p.x), y(p.y),  z( p.z) {}
  point (double x, double y, double z) : x(x), y(y), z(z) {}
  double x, y, z;
};

class PolyLine : public PluginObject
{
public:
  PolyLine ();
  ~PolyLine ();
  void buttonDown (int button, double, double, double);
  bool buttonUp (int button, double, double, double);
  void endObject ();
  bool doubleClick (int button, double, double, double);
  bool hasPoint (double, double, double);
  void removePoint (double, double, double);
private:
  std::vector<point *> pts;
};

PolyLine::PolyLine ()
  :PluginObject (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON)
{
}

void
PolyLine::buttonDown (int buttonDown, double x, double y, double z)
{
}

bool
PolyLine::buttonUp (int buttonUp, double x, double y, double z)
{
  std::cout << "adding point "<< x << " " << y << " " << z << std::endl;
  pts.push_back (new point(x,y,z));
  return true;
}


bool
PolyLine::doubleClick (int buttonUp, double x, double y, double z)
{
  std::cout << "adding point "<< x << " " << y << " " << z << std::endl;
  pts.push_back (new point(x,y,z));
  pts.push_back (new point(*pts[0]));
  endObject ();
  return false;
}


void
PolyLine::endObject ()
{
  
}


bool
hasPoint (double, double, double)
{
}


void
removePoint (double, double, double)
{
}

PolyLine::~PolyLine ()
{
}


DECLARE_PLUGIN (PolyLine);
