#include "gc.h"
#include "math/point.h"
#include "math/vector3.h"

void
GeneralizedCylinder::addPath (PluginObject *o) 
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  path.push_back (o);
}

void
GeneralizedCylinder::addSection (PluginObject *o) 
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  section.push_back (o);
}

void
GeneralizedCylinder::addProfile (PluginObject *o) 
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
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


Vec3f
interpolatepath (const std::vector<Vec3f>& path, float y) 
{
  // assert (y>=0.f && y<=1.0f);
  if (y<0.0f) {
    std::cout << __PRETTY_FUNCTION__ << " / WARNING: y = " << y << std::endl;
    y = 0.0f;
  }
  if (y>1.0f) {
    std::cout << __PRETTY_FUNCTION__ << " / WARNING: y = " << y << std::endl;
    y = 1.0f;
  }
    
  std::cout << "y=" << y << "  ";
  y *= path.size ()-1;
  float inte;
  float fract = std::modf(y, &inte);
  unsigned int a = (int)inte;

  std::cout << "a = " << a << ", size = " << path.size() << std::endl;
  assert (a<path.size() && a>=0);
  if (inte == y)
    return path[a];
  if (fract < std::numeric_limits<float>::epsilon())
    return path[a];
  return Vec3f (path[a].x + (path[a+1].x - path[a].x) * fract,
		path[a].y + (path[a+1].y - path[a].y) * fract,
		0); 
}

void
genTangentes (const std::vector<Vec3f>& normals, std::vector<Vec3f>& tangentes)
{
  std::vector<Vec3f>::const_iterator i;
  std::vector<Vec3f>::const_iterator end=normals.end();
  const Vec3f z(0, 0, 1);
  for (i=normals.begin(); i!=end; ++i)
    tangentes.push_back ((*i).cross(z));    
}
void
GeneralizedCylinder::compute (std::vector<Face>& v)
{
  v.clear ();
  std::vector<Vec3f> vpath;
  std::vector<Vec3f> vnormalpath;
  std::vector<Vec3f> vtangentpath;
  std::vector<Vec3f> vprofile;
  std::vector<float> vtimeprofile;
  std::vector<Vec3f> vsection;
  std::vector<Vec3f> vnormalsection;
  
  std::list<PluginObject *>::iterator ipath = path.begin ();
  std::list<PluginObject *>::iterator iprofile = profile.begin ();
  std::list<PluginObject *>::iterator isection = section.begin ();
  
  std::list<PluginObject *>::iterator iendpath = path.end ();
  std::list<PluginObject *>::iterator iendprofile = profile.end ();
  std::list<PluginObject *>::iterator iendsection = section.end ();

  if (ipath == iendpath)
    return;

  if (iprofile == iendprofile)
    return;

  if (isection == iendsection)
    return;

  std::cout << "GCSTART" << std::endl;

  (*ipath)->evaluate (vpath);
  (*ipath)->evaluateNormals (vnormalpath);

  (*iprofile)->evaluate (vprofile);
  (*iprofile)->evaluateTimeline (vtimeprofile);

  (*isection)->evaluate (vsection);
  (*isection)->evaluateNormals (vnormalsection);

  std::cout << "path size = " << vpath.size () << ", section size = " << vsection.size () << "normals size = " << 
    vnormalsection.size () << std::endl;
  
  for (unsigned int i = 0; i < vprofile.size () - 1; i++) {
    genTangentes(vnormalpath, vtangentpath);
    Vec3f y = vprofile[i];
    y.y = vtimeprofile[i];
    Vec3f y2 = vprofile[i+1];
    y2.y = vtimeprofile[i+1];
    
    float scale1 = y.x;
    float scale2 = y2.x;
    
    Vec3f normale1 = interpolatepath (vnormalpath, y.y);
    Vec3f normale2 = interpolatepath (vnormalpath, y2.y);

    
    float cos1 = Vec3f (1, 0, 0) * normale1;
    float cos2 = Vec3f (1, 0, 0) * normale1;
    float a1, a2;
    if (normale1.y >= 0) 
      a1 = acos (cos1);
    else
      a1 = -acos (cos1);

    if (normale2.y >= 0) 
      a2 = acos (cos2);
    else
      a2 = -acos (cos2);

    std::cout << "timeprofile = " << y.y << ", " << y2.y << std::endl;
    Vec3f pathrel1 = interpolatepath (vpath, y.y);
    Vec3f pathrel2 = interpolatepath (vpath, y2.y);
    pathrel1.z= 0;
    pathrel2.z= 0;
    std::cout << "pathrel1 = " << pathrel1 << std::endl;
    std::cout << "pathrel2 = " << pathrel2 << std::endl;
    for (unsigned int j = 0; j < vsection.size () - 1; j++) {
      Face face;
      face.clear();
      Point p;
      p.setCoords(Vec3f (vsection[j].x, 0, vsection[j].y));
      p*=scale1;
      p+=pathrel1;
      Vec3f n1(vnormalpath[j][0], 0, vnormalpath[j][1] );

      Vec3f n2(vnormalpath[j+1][0], 0, vnormalpath[j+1][1]);

      p.setNormal(n1);
      p.setColor(Vec3f(1,0,0));
      face.push_back (p);

      p.setCoords(Vec3f(vsection[j].x, 0, vsection[j].y));
      p*=scale2;
      p+=pathrel2;
      p.setNormal(n1);
      p.setColor(Vec3f(0,1,0));
      face.push_back (p);

      p.setCoords(Vec3f(vsection[j+1].x, 0, vsection[j+1].y));
      p*=scale2;
      p+=pathrel2;
      p.setNormal (n2);
      p.setColor(Vec3f(0,0,1));
      face.push_back(p);

      v.push_back (face);

      face.clear ();

      p.setCoords(Vec3f(vsection[j].x, 0, vsection[j].y));
      p*=scale1;
      p+=pathrel1;
      p.setNormal(n1);
      p.setColor(Vec3f(1,1,0));
      face.push_back(p);

      p.setCoords(Vec3f (vsection[j+1].x, 0, vsection[j+1].y));
      p*=scale2;
      p+=pathrel2;
      p.setNormal(n2);
      p.setColor(Vec3f(1,0,1));
      face.push_back(p);

      p.setCoords(Vec3f(vsection[j+1].x, 0, vsection[j+1].y));
      p*=scale1;
      p+=pathrel1;
      p.setNormal(n2);
      p.setColor(Vec3f(0,1,1));
      face.push_back(p);

      v.push_back(face);
    }
  }
}
