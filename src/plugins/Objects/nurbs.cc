#include "plugin.h"
#include "../../math/vector3.h"
#include <qevent.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cassert>
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

static void nurbsError(GLenum errorCode)
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf (stderr, "Nurbs Error: %s\n", estring);
}

void
Nurbs::display ()
{
  std::list<Vec3d *>::iterator i;
  std::list<Vec3d *>::iterator end = pts.end ();

  float *knots;
  int nknots;
  float knotsstep;
  float *ctlpoints;
  int nctlpoints;
  int pos;
  int deg = 2;

  std::cout << "nurbs" << std::endl;
 
  GLUnurbsObj* nurbs = gluNewNurbsRenderer();
  assert (nurbs);

  gluNurbsCallback(nurbs, GLU_ERROR, 
		   (GLvoid (*)()) nurbsError);

  nknots = nctlpoints = pts.size ()+(deg)*2;
  knots = new float[nknots];
  ctlpoints = new float[3*nknots];
  
  knotsstep = 1.;
  int c;  
  pos=0;

  for (c = 0, i=pts.begin(); c < deg - 1; c++) {
    knots[pos] = pos;
    ctlpoints[3*pos] = (*i)->x;
    ctlpoints[3*pos+1] = (*i)->y;
    ctlpoints[3*pos+2] = 1.0;
    pos++;
  }
  
  for (i=pts.begin(); i!=end; ++i) {
    //  for (int j=0;j<rep; j++) {
    knots[pos] = pos;
    ctlpoints[3*pos] = (*i)->x;
    ctlpoints[3*pos+1] = (*i)->y;
    ctlpoints[3*pos+2] = 1.0;
    pos++;
    //    }
    std::cout << pos << " (" << ctlpoints[3*pos] << ", " << ctlpoints[3*pos+1]
	      << ", " << ctlpoints[3*pos+2] << ")<" << knots[pos] << ">\n";
  }
  std::list<Vec3d *>::reverse_iterator r = pts.rbegin();
  for (c = 0; c < deg - 1; c++) {
    knots[pos] = pos;
    ctlpoints[3*pos] = (*r)->x;
    ctlpoints[3*pos+1] = (*r)->y;
    ctlpoints[3*pos+2] = 1.0;
    pos++;
  }


  gluBeginCurve (nurbs);
  gluNurbsCurve (nurbs,nknots, knots, 3, ctlpoints, deg + 1,  GL_MAP1_VERTEX_3);
  gluEndCurve (nurbs);
  drawPoints ();
}

DECLARE_PLUGIN (Nurbs);
