#include "plugin.h"
#include "math/vector3.h"
#include <qevent.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cassert>
#include "polyline.h"

#define PLUGIN_NAME "Nurbs"
#define PLUGIN_MENU "Nurbs"
#define PLUGIN_ICON "plugins/Objects/nurbs.png"

class Nurbs : public PolyLine
{
public:
  Nurbs();
  ~Nurbs();
  void display() const;
  float evaluate (std::vector<Vec3f>& res) const;
  void evaluateTimeline (std::vector<float>& time) const;
  void evaluateNormals (std::vector<Vec3f>& normals) const;
  struct nurbs_callback {
    GLenum currenttype;
    Vec3f currentVertex;
    Vec3f currentNormal;
    std::vector<Vec3f>* res;
    std::vector<Vec3f>* norm;
    const Nurbs *pthis;
  };

  void nurbsError (GLenum errorCode) const;
  void beginCallBack (GLenum type, void *data) const;
  void vertexCallBack (GLfloat *vtx, void *data) const;
  void normalCallBack (GLfloat *nml, void *data) const;
  void endCallBack (void *data) const;
  void drawNormals () const;
private:

  bool recompute;
  std::list<Vec3f *>::const_iterator buildCtlPointsVector(const std::list<Vec3f *>& ,
						    std::vector<Vec3f>&) const;
  void buildNodalVector (int order, int size, std::vector<float>& knots) const;
  mutable std::vector<Vec3f> points;
  mutable std::vector<Vec3f> normals;
  mutable std::vector<float> timeline;
  void compute () const;
};

Nurbs::Nurbs () : PolyLine (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON),
		  recompute(false)
{
  std::cout << PLUGIN_NAME << std::endl;
}

Nurbs::~Nurbs()
{
}

void
Nurbs::nurbsError(GLenum errorCode) const
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf (stderr, "Nurbs Error: %s\n", estring);
}

void
Nurbs::buildNodalVector (int ordre, int size, std::vector<float>& knots) const
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

std::list<Vec3f *>::const_iterator
Nurbs::buildCtlPointsVector (const std::list<Vec3f *>& source,
		      std::vector<Vec3f>& ctlpoints) const
{
  std::list<Vec3f *>::const_iterator p;
  std::list<Vec3f *>::const_iterator pend = pts.end ();

  std::vector<Vec3f>::const_iterator j;
  std::vector<Vec3f>::const_iterator jend;

  std::list<Vec3f *>::const_iterator nearest;


  float mindist = std::numeric_limits<float>::max ();

  /* build ctl points vector */
  p = pts.begin ();
  if (p == pend)
    return p;

  while (p != pend) {
    ctlpoints.push_back (Vec3f (**p));
    std::list<Vec3f *>::const_iterator first = p++;

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
  return nearest;
}

static void
nurbsError (GLenum errorCode, const void *data ) 
{
  const Nurbs *n = ((struct Nurbs::nurbs_callback *)(data))->pthis;
  n->nurbsError (errorCode);
}
static void
beginCallBack (GLenum type, void *data) 
{
  const Nurbs *n = ((struct Nurbs::nurbs_callback *)data)->pthis;
  n->beginCallBack (type, data);
}
static void
vertexCallBack (GLfloat *vtx, void *data)
{
   const Nurbs *n = ((struct Nurbs::nurbs_callback *)data)->pthis;
   n->vertexCallBack (vtx, data);
}
static void
normalCallBack (GLfloat *nml, void *data)
{
   const Nurbs *n = ((struct Nurbs::nurbs_callback *)data)->pthis;
   n->normalCallBack (nml, data);
}
static void
endCallBack (void *data)
{
   const Nurbs *n = ((struct Nurbs::nurbs_callback *)data)->pthis;
   n->endCallBack (data);
}

void
Nurbs::display () const
{
  std::vector<float> knots;
  std::vector<Vec3f> ctlpoints;
  std::list<Vec3f *>::const_iterator nearest;
  
  unsigned int deg = 1;
  int ordre;
  nearest = buildCtlPointsVector ( pts, ctlpoints);
  GLUnurbsObj* nurbs = gluNewNurbsRenderer();
  assert (nurbs);

  gluNurbsCallback(nurbs, GLU_ERROR, (void (*)())(::nurbsError));

  if (deg < ctlpoints.size () - 1)
    deg = ctlpoints.size () - 1;
  ordre = deg + 1; 
  if (ctlpoints.size ()>=2) {
    if (ordre >=6)
      ordre = 6;
    
    buildNodalVector (ordre, ctlpoints.size (), knots);
    glColor3f (.0,.0,.0);
    gluBeginCurve (nurbs);
    gluNurbsCurve (nurbs,knots.size (), &knots[0], 3, 
		   &ctlpoints[0][0], ordre,  GL_MAP1_VERTEX_3);
    gluEndCurve (nurbs);
  }
  gluDeleteNurbsRenderer (nurbs);
  drawPoints (nearest);
}

void
Nurbs::beginCallBack (GLenum type, void *data) const {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  switch (type) {
  case GL_LINES:
    ncb->currenttype = GL_LINES;
    break;
  case GL_LINE_LOOP:
    ncb->currenttype = GL_LINE_LOOP;
    break;
  case GL_LINE_STRIP:
    ncb->currenttype = GL_LINE_STRIP;
    break;
  default:
    break;
  }
}
void
Nurbs::vertexCallBack (GLfloat *vtx, void *data) const {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  ncb->res->push_back (ncb->currentVertex = Vec3f (vtx[0], vtx[1], vtx[2]));
}

void
Nurbs::normalCallBack (GLfloat *nml, void *data) const {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  ncb->norm->push_back (ncb->currentNormal = Vec3f (nml[0], nml[1], nml[2]));
}

void
Nurbs::endCallBack (void *data)  const {
  struct nurbs_callback *ncb = (struct nurbs_callback *)data;
  if (ncb->currenttype == GL_LINE_LOOP) {
    ncb->res->push_back (ncb->currentVertex);
    ncb->norm->push_back (ncb->currentNormal);
  }
}

void
Nurbs::compute () const
{
  std::vector<float> knots;
  std::vector<Vec3f> ctlpoints;
  std::vector<Vec3f> ctlnormals;
  PolyLine::evaluateNormals (ctlnormals);
  struct nurbs_callback ncb;
  ncb.res = &points;
  ncb.norm = &normals;
  ncb.pthis = this;
  unsigned int deg = 1;
  int ordre;

  assert (resolution);

  /*  for (int i=0;i<ctlnormals.size (); i++)
      ctlpoints.push_back (ctlnormals[i]);*/
  std::cout << "WARNING RECOMPUTING\n" << std::endl;

  points.clear ();
  normals.clear ();
  timeline.clear ();

  buildCtlPointsVector ( pts, ctlpoints);
  
  GLUnurbsObj* nurbs = gluNewNurbsRenderer();
  assert (nurbs);
  gluNurbsProperty (nurbs, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);
  gluNurbsProperty(nurbs, GLU_U_STEP, resolution);
  gluNurbsProperty(nurbs, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
  
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
  for (unsigned int c = 0; c<points.size (); c++) {
    timeline.push_back (t);
    t+=tstep;
    
  }
  need_recompute=false;
}

float
Nurbs::evaluate (std::vector<Vec3f>& res) const
{
  std::vector<Vec3f> tmp;
  float len = PolyLine::evaluate (tmp);
  if (need_recompute)
    compute ();
  res = points;
  return len;
}

void
Nurbs::evaluateTimeline (std::vector<float>& t) const
{
  if (need_recompute)
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

  for (unsigned int i=0;i<t.size (); i++)
    t[i] = (t[i] - miny)/ (maxy-miny);  
}

void
Nurbs::evaluateNormals (std::vector<Vec3f>& n) const
{
  if (need_recompute)
    compute();
  n = normals;
}

void
Nurbs::drawNormals () const
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

DECLARE_PLUGIN (Nurbs);
