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
  std::list<Vec3f *>::iterator buildCtlPointsVector(const std::list<Vec3f *>& ,
						    std::vector<Vec3f>&);
  void buildNodalVector (int order, int size, std::vector<float>& knots);
  void evaluate (int ,std::vector<Vec3f>&);
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
Nurbs::buildNodalVector (int ordre, int size, std::vector<float>& knots)
{
  std::vector<float>::iterator i;
  std::vector<float>::iterator iend;

  if (ordre >= 2) {
    int c;  
    
    // ajoute ordre 0
    // ajoute 1 à nctlpoints - ordre
    // ajoute ordre (nctlpoints - ordre)
    
    for (c = 0; c < ordre; c++)
      knots.push_back (0);
    
    for (c = 0; c < size - ordre; c++)
      knots.push_back (c+1);
    
    for (c = 0; c < ordre; c++)
	knots.push_back (size - ordre + 1);
    
    std::cout << "[";
    for (i = knots.begin (), iend = knots.end ();
	 i != iend; ++i)
      std::cout << *i << ", ";
    std::cout << "]" << std::endl;
        
  }
}

std::list<Vec3f *>::iterator
Nurbs::buildCtlPointsVector (const std::list<Vec3f *>& source,
		      std::vector<Vec3f>& ctlpoints)
{
  std::list<Vec3f *>::iterator p;
  std::list<Vec3f *>::iterator pend = pts.end ();

  std::vector<Vec3f>::iterator j;
  std::vector<Vec3f>::iterator jend;

  std::list<Vec3f *>::iterator nearest;


  float mindist = std::numeric_limits<float>::max ();

  std::cout << "nurbs" << std::endl;


  /* build ctl points vector */
  p = pts.begin ();
  if (p == pend)
    return p;

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
  std::cout << "[";
  for (j = ctlpoints.begin (), jend = ctlpoints.end ();
       j != jend; ++j)
    std::cout << "(" << *j << "), ";
  std::cout << "]" << std::endl;

}

void
Nurbs::display ()
{
  std::vector<float> knots;
  std::vector<Vec3f> ctlpoints;
  std::list<Vec3f *>::iterator nearest;
  
  int deg = 1;
  int ordre;
  nearest = buildCtlPointsVector ( pts, ctlpoints);
  
  GLUnurbsObj* nurbs = gluNewNurbsRenderer();
  assert (nurbs);

  gluNurbsCallback(nurbs, GLU_ERROR, 
		   (GLvoid (*)()) nurbsError);

  if (deg < ctlpoints.size () - 1)
    deg = ctlpoints.size () - 1;
  ordre = deg + 1; 
  std::cout << "points " << ctlpoints.size () << std::endl;
  std::cout << "deg " << deg << ", ordre " << ordre << std::endl;
  if (ctlpoints.size ()>=2) {
    if (ordre >=6)
      ordre = 6;
    
    buildNodalVector (ordre, ctlpoints.size (), knots);
    gluBeginCurve (nurbs);
    gluNurbsCurve (nurbs,knots.size (), &knots[0], 3, 
		   &ctlpoints[0][0], ordre,  GL_MAP1_VERTEX_3);
    gluEndCurve (nurbs);
  }
  gluDeleteNurbsRenderer (nurbs);
  drawPoints (nearest);
}


struct nurbs_callback {
  GLenum currenttype;
  Vec3f currentVertex;
  std::vector<Vec3f>* res;
};

static void beginCallBack (GLenum type, void *data) {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  switch (type) {
  case GL_LINES:
    std::cout << "Lines";
    ncb->currenttype = GL_LINES;
    break;
  case GL_LINE_LOOP:
    std::cout << "LL";
    ncb->currenttype = GL_LINE_LOOP;
    break;
  case GL_LINE_STRIP:
    std::cout << "LS";
    ncb->currenttype = GL_LINE_STRIP;
    break;
  default:
    abort ();
  }
}
static void vertexCallBack (GLfloat *vtx, void *data) {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  std::cout << ".";
  ncb->res->push_back (Vec3f (vtx[0], vtx[1], vtx[2]));
}
static void normalCallBack (GLfloat *nml) {
  abort ();
}
static void endCallBack (void *data) {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  if (ncb->currenttype == GL_LINE_LOOP) {
    std::cout << "+";
    ncb->res->push_back (ncb->currentVertex);
  }
  std::cout << "E";
}

void
Nurbs::evaluate (int resolution, std::vector<Vec3f>& res)
{

  std::vector<float> knots;
  std::vector<Vec3f> ctlpoints;
  struct nurbs_callback ncb;
  ncb.res = &res;
  int deg = 1;
  int ordre;
  
  buildCtlPointsVector ( pts, ctlpoints);
  
  GLUnurbsObj* nurbs = gluNewNurbsRenderer();
  assert (nurbs);
  gluNurbsProperty (nurbs, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);
  
  gluNurbsCallback(nurbs, GLU_ERROR, 
		   (GLvoid (*)()) nurbsError);

  gluNurbsCallback(nurbs, GLU_NURBS_BEGIN_DATA, (void (*)())beginCallBack);
  gluNurbsCallback(nurbs, GLU_NURBS_VERTEX_DATA, (void (*)())vertexCallBack);
  //  gluNurbsCallback(nurbs, GLU_NURBS_NORMAL, normalCallBack);
  gluNurbsCallback(nurbs, GLU_NURBS_END_DATA, (void (*)())endCallBack);

  if (deg < ctlpoints.size () - 1)
    deg = ctlpoints.size () - 1;
  ordre = deg + 1; 
  std::cout << "points " << pts.size () << std::endl;
  std::cout << "deg " << deg << ", ordre " << ordre << std::endl;
  if (ctlpoints.size ()>=2) {
    if (ordre >=6)
      ordre = 6;
    
    buildNodalVector (ordre, ctlpoints.size (), knots);
    gluBeginCurve (nurbs);
    gluNurbsCurve (nurbs,knots.size (), &knots[0], 3, 
		   &ctlpoints[0][0], ordre,  GL_MAP1_VERTEX_3);
    gluEndCurve (nurbs);
  }
  gluDeleteNurbsRenderer (nurbs);
  
}

DECLARE_PLUGIN (Nurbs);
