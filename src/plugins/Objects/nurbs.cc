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

  std::list<Vec3f *>::iterator p;
  std::list<Vec3f *>::iterator pend = pts.end ();

  std::vector<Vec3f>::iterator j;
  std::vector<Vec3f>::iterator jend;

  std::vector<float>::iterator i;
  std::vector<float>::iterator iend;


  std::vector<float> knots;
  std::vector<Vec3f> ctlpoints;
  std::list<Vec3f *>::iterator nearest;
  float mindist = std::numeric_limits<float>::max ();

  p = pts.begin ();
  if (p == pend)
    return;

  while (p != pend) {
    ctlpoints.push_back (Vec3f (**p));
    std::list<Vec3f *>::iterator first = p++;

    if (first == pend)
      break;
    if (p != pend) {
      float dist = distanceToSegment (Vec3f (cursor.x, cursor.y, 0),
				      **first, **p);
      if (dist < mindist) {
	mindist = dist;
	nearest = first;
      }
    }
  }

  int deg = 1;
  int ordre;
  
  std::cout << "nurbs" << std::endl;
  
  GLUnurbsObj* nurbs = gluNewNurbsRenderer();
  assert (nurbs);

  gluNurbsCallback(nurbs, GLU_ERROR, 
		   (GLvoid (*)()) nurbsError);

  if (deg < ctlpoints.size () - 1)
    deg = ctlpoints.size () - 1;
  ordre = deg + 1; 
  std::cout << "points " << pts.size () << std::endl;
  std::cout << "deg " << deg << ", ordre " << ordre << std::endl;
  if (ctlpoints.size ()>=2) {
    if (ordre >=4)
      ordre = 4;
    if (ordre >= 2) {
      int c;  

      // ajoute ordre 0
      // ajoute 1 à nctlpoints - ordre
      // ajoute ordre (nctlpoints - ordre)

      for (c = 0; c < ordre; c++)
	knots.push_back (0);

      for (c = 0; c < ctlpoints.size () - ordre; c++)
	knots.push_back (c+1);

      for (c = 0; c < ordre; c++)
	knots.push_back (ctlpoints. size () - ordre + 1);

      std::cout << "------------------" << std::endl;
      std::cout << "deg = " << deg << std::endl << "ordre = " 
		<< ordre << std::endl << "[";
      for (i = knots.begin (), iend = knots.end ();
	   i != iend; ++i)
	std::cout << *i << ", ";
      std::cout << "]" << std::endl;

      for (j = ctlpoints.begin (), jend = ctlpoints.end ();
	   j != jend; ++j)
	std::cout << "(" << *j << "), ";
      std::cout << "]" << std::endl;
      
      gluBeginCurve (nurbs);
      gluNurbsCurve (nurbs,knots.size (), &knots[0], 3, 
		     &ctlpoints[0][0], ordre,  GL_MAP1_VERTEX_3);
      gluEndCurve (nurbs);
    }
  }
  gluDeleteNurbsRenderer (nurbs);
  drawPoints (nearest);
}

DECLARE_PLUGIN (Nurbs);
