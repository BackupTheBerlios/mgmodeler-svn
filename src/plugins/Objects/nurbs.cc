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
  void evaluate (std::vector<Vec3f>& res);
  void evaluateTimeline (std::vector<float>& time);
  void evaluateNormals (std::vector<Vec3f>& normals);
  struct nurbs_callback {
    GLenum currenttype;
    Vec3f currentVertex;
    Vec3f currentNormal;
    std::vector<Vec3f>* res;
    std::vector<Vec3f>* norm;
    Nurbs *pthis;
  };

  void nurbsError (GLenum errorCode);
  void beginCallBack (GLenum type, void *data);
  void vertexCallBack (GLfloat *vtx, void *data);
  void normalCallBack (GLfloat *nml, void *data);
  void endCallBack (void *data);

private:

  bool recompute;
  void buildNodalVector (int order, int size, std::vector<float>& knots);
  std::vector<Vec3f> points;
  std::vector<Vec3f> normals;
  std::vector<float> timeline;
  void compute ();
};

Nurbs::Nurbs () : PolyLine (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON),
		  recompute(false)
{}

Nurbs::~Nurbs()
{
}

void
Nurbs::nurbsError(GLenum errorCode)
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

static void
nurbsError (GLenum errorCode, const void *data ) 
{
  Nurbs *n = ((struct Nurbs::nurbs_callback *)(data))->pthis;
  n->nurbsError (errorCode);
}
static void
beginCallBack (GLenum type, void *data) 
{
  Nurbs *n = ((struct Nurbs::nurbs_callback *)data)->pthis;
  n->beginCallBack (type, data);
}
static void
vertexCallBack (GLfloat *vtx, void *data)
{
   Nurbs *n = ((struct Nurbs::nurbs_callback *)data)->pthis;
   n->vertexCallBack (vtx, data);
}
static void
normalCallBack (GLfloat *nml, void *data)
{
   Nurbs *n = ((struct Nurbs::nurbs_callback *)data)->pthis;
   n->normalCallBack (nml, data);
}
static void
endCallBack (void *data)
{
   Nurbs *n = ((struct Nurbs::nurbs_callback *)data)->pthis;
   n->endCallBack (data);
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

  gluNurbsCallback(nurbs, GLU_ERROR, (void (*)())(::nurbsError));

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

void
Nurbs::beginCallBack (GLenum type, void *data) {
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
void
Nurbs::vertexCallBack (GLfloat *vtx, void *data) {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  std::cout << "p";
  ncb->res->push_back (ncb->currentVertex = Vec3f (vtx[0], vtx[1], vtx[2]));
}

void
Nurbs::normalCallBack (GLfloat *nml, void *data) {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  std::cout << "n";
  ncb->norm->push_back (ncb->currentNormal = Vec3f (nml[0], nml[1], nml[2]));
}

void
Nurbs::endCallBack (void *data) {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  if (ncb->currenttype == GL_LINE_LOOP) {
    std::cout << "(+np)";
    ncb->res->push_back (ncb->currentVertex);
    ncb->norm->push_back (ncb->currentNormal);
  }
  std::cout << "E";
}

void
Nurbs::compute ()
{

  std::vector<float> knots;
  std::vector<Vec3f> ctlpoints;
  std::vector<Vec3f> ctlnormals;
  PolyLine::evaluateNormals (ctlnormals);
  struct nurbs_callback ncb;
  ncb.res = &points;
  ncb.norm = &normals;
  ncb.pthis = this;
  int deg = 1;
  int ordre;

  assert (resolution);

  /*  for (int i=0;i<ctlnormals.size (); i++)
      ctlpoints.push_back (ctlnormals[i]);*/


  glPushMatrix ();
  glLoadIdentity();
  points.clear ();
  normals.clear ();
  timeline.clear ();

  buildCtlPointsVector ( pts, ctlpoints);
  
  GLUnurbsObj* nurbs = gluNewNurbsRenderer();
  assert (nurbs);
  gluNurbsProperty (nurbs, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);
  
  gluNurbsCallback(nurbs, GLU_ERROR, 
		   (GLvoid (*)()) (::nurbsError));
  gluNurbsCallbackData (nurbs, (void *)&ncb);
  gluNurbsCallback(nurbs, GLU_NURBS_BEGIN_DATA, (void (*)())::beginCallBack);
  gluNurbsCallback(nurbs, GLU_NURBS_VERTEX_DATA, (void (*)())::vertexCallBack);
  gluNurbsCallback(nurbs, GLU_NURBS_NORMAL_DATA, (void (*)())::normalCallBack);
  gluNurbsCallback(nurbs, GLU_NURBS_END_DATA, (void (*)())::endCallBack);

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
    gluNurbsCurve (nurbs,knots.size (), &knots[0], 3, 
		   &ctlnormals[0][0], ordre,  GL_MAP1_NORMAL);

    gluEndCurve (nurbs);
  }
  gluDeleteNurbsRenderer (nurbs);
  float t = 0;
  float tstep = 1.0f / (points.size () - 1);
  for (int c = 0; c<points.size (); c++) {
    std::cout << t << std::endl;
    timeline.push_back (t);
    t+=tstep;
    
  }
  glPopMatrix ();
}

void
Nurbs::evaluate (std::vector<Vec3f>& res)
{
  compute ();
  res = points;
}

void
Nurbs::evaluateTimeline (std::vector<float>& t)
{
  compute ();
  float miny = std::numeric_limits<float>::max();
  float maxy = std::numeric_limits<float>::min();

  std::vector<Vec3f>::iterator i;
  std::vector<Vec3f>::iterator end;
  
  t.clear ();

  if (points.empty ())
    return;


  end = points.end ();

  for (i = points.begin (); i != end; ++i) {
    float y = (*i).y;
    t.push_back (y);
    if (miny>y)
      miny = y;
    if (maxy<y)
      maxy = y;
  }

  for (int i=0;i<t.size (); i++)
    t[i] = (t[i] - miny)/ (maxy-miny);  
}

void
Nurbs::evaluateNormals (std::vector<Vec3f>& n)
{
  compute();
  n = normals;
}


DECLARE_PLUGIN (Nurbs);
