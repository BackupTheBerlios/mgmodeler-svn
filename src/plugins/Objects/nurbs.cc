#include "plugin.h"
#include "../../math/vector3.h"
#include <qevent.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

#include "polyline.h"

#define PLUGIN_NAME "Nurbs Plugin"
#define PLUGIN_MENU "object/polylines"
#define PLUGIN_ICON "plugins/Objects/nurbs.png"




class Nurbs : public PolyLine
{
public:
  Nurbs();
  ~Nurbs();
  void display();
};

Nurbs::Nurbs () : PolyLine (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON)
{
}

Nurbs::~Nurbs()
{
}

void
Nurbs::display ()
{
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();

  std::cout << "nurbs" << std::endl;
  glBegin (GL_LINE_STRIP);
  for (i=pts.begin(); i!=end; ++i) {
    std::cout << "pt(" << (*i)->x << ", " << (*i)->y << ")\n";
    glVertex2f ((*i)->x, -(*i)->y); 
  }
  glEnd ();
}

DECLARE_PLUGIN (Nurbs);
