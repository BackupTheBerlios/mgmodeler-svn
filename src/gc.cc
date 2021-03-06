#include "gc.h"
#include "math/point.h"
#include "math/vector3.h"

void
GeneralizedCylinder::addPath (PluginObject *o) 
{
  path.push_back (o);
}

void
GeneralizedCylinder::addSection (PluginObject *o) 
{
  section.push_back (o);
}

void
GeneralizedCylinder::addProfile (PluginObject *o) 
{
  profile.push_back (o);
}


void
GeneralizedCylinder::removePath (PluginObject *o) 
{
  path.remove (o);
}

void
GeneralizedCylinder::removeSection (PluginObject *o) 
{
  section.remove (o);
}

void
GeneralizedCylinder::removeProfile (PluginObject *o) 
{
  profile.remove (o);
}

void
GeneralizedCylinder::cfgDialog ()
{
}

void
GeneralizedCylinder::setNormalScale(float ns)
{
  normale_scale=ns;
}

class Path {
public:
  Path (const PluginObject *init) {
    length=init->evaluate(path);
    init->evaluateNormals(normals);
  }
  
  ~Path () {
  }
  
  const Vec3f interpolate (const std::vector<Vec3f>& which, float where) const {
    if (where<0.0f)
      where = 0.0f;
    
    if (where>1.0f)
      where = 1.0f;
    
    where *= which.size ()-1;
    float inte;
    float fract = std::modf(where, &inte);
    unsigned int a = (int)inte;

    assert (a<which.size() && a>=0);
    if (inte == where || fract < std::numeric_limits<float>::epsilon())
      return Vec3f(which[a]);
    return Vec3f (which[a].x + (which[a+1].x - which[a].x) * fract,
		  which[a].y + (which[a+1].y - which[a].y) * fract,
		  0); 
  }
    
  static void rotate (const Vec3f& tangent, const Vec3f& normal, const Vec3f& in, Vec3f& out) {
    out.x = in.x*normal.x + in.y*normal.y + in.z*normal.z;
    out.y = in.x*tangent.x + in.y*tangent.y + in.z*tangent.z;
    out.z = in.x*binormal.x + in.y*binormal.y + in.z*binormal.z;
  }

  Point changeReference (float where, const Point& p, Vec3f& translate) const {
    Vec3f normal(interpolate(normals, where));
    normal.normalize();
    const Vec3f tangent(normal.y, -normal.x, 0);

    Point res;
    rotate (tangent, normal, p.getCoords(), res.getCoords());
    translate = interpolate(path, where);

    if (translate.z!=0.0) 
      translate.z=0;

    rotate (tangent, normal, p.getNormal(), res.getNormal());

    return res;
  }
  friend class WholePath;
private:  
  static Vec3f binormal;
  float length;
  std::vector<Vec3f> path;
  std::vector<Vec3f> normals;
};

Vec3f Path::binormal(0, 0, 1);

class WholePath {
  struct bound {
    bound(float min, float max) : min(min), max(max) {}
    float min, max;
  };
public:
  void add(const Path& p) {
    paths.push_back (p);
  }
  void computeBounds () {
    float last = 0;
    std::vector<Path>::const_iterator i;
    std::vector<Path>::const_iterator iend = paths.end();
    for (i=paths.begin(); i!=iend; ++i) {
      bound b(last, last+i->length);
      last+=i->length;
      bounds.push_back(b);
    }

    std::vector<bound>::iterator j;
    std::vector<bound>::iterator jend = bounds.end();
    assert (last);
    for (j=bounds.begin(); j!=jend; ++j) {
      j->min/=last;
      j->max/=last;
    }
  }
  int changeReference (float where, Point& p, Vec3f& translate) const {
    if (where<0.0f)
      where = 0.0f;
    if (where>1.0f)
      where = 1.0f;
    
    for (unsigned int i=0; i<bounds.size (); i++) {
      if (where >= bounds[i].min && where<= bounds[i].max) {
	Vec3f color=p.getColor();
	p=paths[i].changeReference ((where-bounds[i].min)/
				      (bounds[i].max-bounds[i].min), p, translate);
	p.setColor(color);
	return i;
      }
    }
    throw "no matching part\n";
  }
  
private:
  std::vector<Path> paths;
  std::vector<bound> bounds;
};

void
GeneralizedCylinder::compute ()
{
  compute (m_faces);
}

void
GeneralizedCylinder::compute (std::vector<Face>& v)
{
  v.clear ();

  WholePath wpath;

  std::vector<Vec3f> vprofile;
  std::vector<Vec3f> vnormalprofile;
  std::vector<float> vtimeprofile;
  std::vector<Vec3f> vsection;
  std::vector<Vec3f> vnormalsection;
  
  std::list<PluginObject *>::const_iterator ipath = path.begin ();
  std::list<PluginObject *>::iterator iprofile = profile.begin ();
  std::list<PluginObject *>::iterator isection = section.begin ();
  
  std::list<PluginObject *>::iterator iendpath = path.end ();
  std::list<PluginObject *>::iterator iendprofile = profile.end ();
  std::list<PluginObject *>::iterator iendsection = section.end ();

  if (ipath == iendpath || iprofile == iendprofile || isection == iendsection)
    return;

  v.clear ();

  while (ipath!=iendpath) {
    (*ipath)->setResolution (rPath);
    wpath.add (Path(*ipath));
    ++ipath;
  }
  wpath.computeBounds ();

  (*iprofile)->setResolution(rProfile);
  (*iprofile)->evaluate (vprofile);
  (*iprofile)->evaluateNormals (vnormalprofile);
  (*iprofile)->evaluateTimeline (vtimeprofile);

  int last = 0;
  for (unsigned int i = 0; i < vprofile.size () - 1; i++) {
    float scale1 = vprofile[i].x;
    float scale2 = vprofile[i+1].x;
    std::list<PluginObject *>::iterator isection = section.begin ();
    for (; isection!=iendsection; ++isection) {
      (*isection)->setResolution(rSection);
      (*isection)->evaluate (vsection);
      (*isection)->evaluateNormals (vnormalsection);
      
      Vec3f tangent1(vnormalprofile[i].y,
		     -vnormalprofile[i].x, 0);
      Vec3f tangent2(vnormalprofile[i+1].y,
		     -vnormalprofile[i+1].x, 0);

    for (unsigned int j = 0; j < vsection.size () - 1; j++) {
      Face face;
      Point p;

      Vec3f np1(vnormalsection[j].x, vnormalsection[j].z,
	       vnormalsection[j].y);
      np1*=normale_scale;
      Vec3f np2(vnormalsection[j+1].x, vnormalsection[j+1].z,
	       vnormalsection[j+1].y);
      np2*=normale_scale;
      
      Vec3f n1(np1),n2(np2);
      /*      Path::rotate (tangent1, vnormalprofile[i], np1, n2);
	      Path::rotate (tangent2, vnormalprofile[i+1], np2, n1);*/
      n1.normalize ();
      n2.normalize ();

      p.setCoords(Vec3f (vsection[j].x, 0, vsection[j].y));
      p.setNormal(n1);
      p.setColor(n1);
      Vec3f pos;
      last=wpath.changeReference (vtimeprofile[i], p, pos);
      p*=scale1;
      p+=pos;
      face.push_back (p);

      p.setCoords(Vec3f(vsection[j].x, 0, vsection[j].y));
      p.setNormal(n1);
      p.setColor(n1);
      if (last!=wpath.changeReference (vtimeprofile[i+1], p, pos))
	continue;
      p*=scale2;
      p+=pos;
      face.push_back (p);

      p.setCoords(Vec3f(vsection[j+1].x, 0, vsection[j+1].y));
      p.setNormal (n2);
      p.setColor(n2);
      last=wpath.changeReference (vtimeprofile[i+1], p, pos);
      p*=scale2;
      p+=pos;
      face.push_back(p);

      v.push_back (face);
      face.clear ();

      p.setCoords(Vec3f(vsection[j].x, 0, vsection[j].y));
      p.setNormal(n1);
      p.setColor(n1);
      last=wpath.changeReference (vtimeprofile[i], p, pos);
      p*=scale1;
      p+=pos;
      face.push_back(p);

      p.setCoords(Vec3f (vsection[j+1].x, 0, vsection[j+1].y));
      p.setNormal(n2);
      p.setColor(n2);
      last=wpath.changeReference (vtimeprofile[i+1], p, pos);
      p*=scale2;
      p+=pos;
      face.push_back(p);

      p.setCoords(Vec3f(vsection[j+1].x, 0, vsection[j+1].y));
      p.setNormal(n2);
      p.setColor(n2);
      last=wpath.changeReference (vtimeprofile[i], p, pos);
      p*=scale1;
      p+=pos;
      face.push_back(p);

      v.push_back(face);
    }
    }
  }
}

void
GeneralizedCylinder::setResolution(Curve c, int u) {
  switch (c) {
  case PROFILE:
    rProfile = u;
    break;
  case SECTION:
    rSection = u;
    break;
  case PATH:
    rPath = u;
    break;
  }
}
