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

void
GeneralizedCylinder::setNormalScale(float ns)
{
  normale_scale=ns;
}

namespace {
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
  changeRepere (const Vec3f& tangent, const Vec3f& normal, 
		const Vec3f& binormal, const Vec3f& in, Vec3f& out)
  {
    out.x = in.x*normal.x + in.y*normal.y + in.z*normal.z;
    out.y  = in.x*tangent.x + in.y*tangent.y + in.z*tangent.z;
    out.z = in.x*binormal.x + in.y*binormal.y + in.z*binormal.z;
  }

  void 
  pointToPath (const Vec3f& tangent, const Vec3f& normal,
	       const Vec3f& binormal, Point& p) {
    Vec3f coords=p.getCoords();
    Vec3f norm=p.getNormal();
    changeRepere (tangent, normal, binormal, coords, p.getCoords());
    changeRepere (tangent, normal, binormal, norm, p.getNormal());
  }
}
void
GeneralizedCylinder::compute (std::vector<Face>& v)
{
  v.clear ();

  std::vector<Vec3f> vpath;
  std::vector<Vec3f> vnormalpath;
  std::vector<Vec3f> vtangentpath;
  std::vector<Vec3f> vprofile;
  std::vector<Vec3f> vnormalprofile;
  std::vector<float> vtimeprofile;
  std::vector<Vec3f> vsection;
  std::vector<Vec3f> vnormalsection;
  
  std::list<PluginObject *>::iterator ipath = path.begin ();
  std::list<PluginObject *>::iterator iprofile = profile.begin ();
  std::list<PluginObject *>::iterator isection = section.begin ();
  
  std::list<PluginObject *>::iterator iendpath = path.end ();
  std::list<PluginObject *>::iterator iendprofile = profile.end ();
  std::list<PluginObject *>::iterator iendsection = section.end ();

  if (ipath == iendpath || iprofile == iendprofile || isection == iendsection)
    return;

  std::cout << "GCSTART" << std::endl;

  (*ipath)->evaluate (vpath);
  (*ipath)->evaluateNormals (vnormalpath);

  (*iprofile)->evaluate (vprofile);
  (*iprofile)->evaluateNormals (vnormalprofile);
  (*iprofile)->evaluateTimeline (vtimeprofile);

  (*isection)->evaluate (vsection);
  (*isection)->evaluateNormals (vnormalsection);

  for (unsigned int i = 0; i < vprofile.size () - 1; i++) {
    std::cout << "---------------" << i << "-------------------\n";
    float scale1 = vprofile[i].x;
    float scale2 = vprofile[i+1].x;
    
    Vec3f normale1 = interpolatepath (vnormalpath, vtimeprofile[i]);
    Vec3f normale2 = interpolatepath (vnormalpath, vtimeprofile[i+1]);
    normale1.normalize();
    normale2.normalize();
    Vec3f tangent1(normale1.y, -normale1.x, 0);
    Vec3f tangent2(normale2.y, -normale2.x, 0);
    tangent1.normalize();
    tangent2.normalize();
    Vec3f position1 = interpolatepath (vpath, vtimeprofile[i]);
    Vec3f position2 = interpolatepath (vpath, vtimeprofile[i+1]);

    std::cout << "position1(" << position1 << ")\n";
    std::cout << "position2(" << position2 << ")\n";

    position1.z= position2.z= 0;
    Vec3f binormal(0, 0, 1);

    for (unsigned int j = 0; j < vsection.size () - 1; j++) {
      Face face;
      Point p;

      Vec3f n1(vnormalsection[j].x, vnormalsection[j].z,
	       vnormalsection[j].y);
      n1*=normale_scale;
      Vec3f n2(vnormalsection[j+1].x, vnormalsection[j+1].z,
	       vnormalsection[j+1].y);
      n2*=normale_scale;

      p.setCoords(Vec3f (vsection[j].x, 0, vsection[j].y));
      p.setNormal(n1);
      p.setColor(n1);
      std::cout << "p(" << p.getCoords() << "), n(" << normale1 << "), t(" << tangent1 << "), b(" << binormal << ")\n";
      pointToPath(tangent1, normale1, binormal, p);
      std::cout << "p'" << p.getCoords() << ")\n" ;
      p*=scale1;
      p+=position1;
      face.push_back (p);

      p.setCoords(Vec3f(vsection[j].x, 0, vsection[j].y));
      p.setNormal(n1);
      p.setColor(n1);
      pointToPath(tangent2, normale2, binormal, p);
      p*=scale2;
      p+=position2;
      face.push_back (p);

      p.setCoords(Vec3f(vsection[j+1].x, 0, vsection[j+1].y));
      p.setNormal (n2);
      p.setColor(n2);
      pointToPath(tangent2, normale2, binormal, p);
      p*=scale2;
      p+=position2;
      face.push_back(p);

      v.push_back (face);
      face.clear ();

      p.setCoords(Vec3f(vsection[j].x, 0, vsection[j].y));
      p.setNormal(n1);
      p.setColor(n1);
      pointToPath(tangent1, normale1, binormal, p);
      p*=scale1;
      p+=position1;
      face.push_back(p);

      p.setCoords(Vec3f (vsection[j+1].x, 0, vsection[j+1].y));
      p.setNormal(n2);
      p.setColor(n2);
      pointToPath(tangent2, normale2, binormal, p);
      p*=scale2;
      p+=position2;
      face.push_back(p);

      p.setCoords(Vec3f(vsection[j+1].x, 0, vsection[j+1].y));
      p.setNormal(n2);
      p.setColor(n2);
      pointToPath (tangent1, normale1, binormal, p);
      p*=scale1;
      p+=position1;
      face.push_back(p);

      v.push_back(face);
    }
  }
}
