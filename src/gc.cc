#include "gc.h"
#include "view3d.h"
/*GeneralizedCylinder::GeneralizedCylinder () {
}

GeneralizedCylinder::~GeneralizedCylinder () {
}*/

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

#if 0
Vec3f
getNormalizedProfile (const std::vector<Vec3f>& vprofile, int y) 
{
  float miny = std::numeric_limits<float>::max() ;
  float maxy = std::numeric_limits<float>::min() ;

  for (unsigned int i = 0; i < vprofile.size (); i++) {
    if (vprofile[i].y < miny)
      miny = vprofile[i].y;
    if (maxy < vprofile[i].y)
      maxy = vprofile[i].y;
  }

  return Vec3f (vprofile[y].x, (vprofile[y].y-miny)/(maxy-miny), vprofile[y].z);
}

#endif

void
GeneralizedCylinder::display () 
{
  std::vector<std::vector<Vec3f> > object;
  std::vector<std::vector<Vec3f> > normals;

  std::vector<Vec3f> vpath;
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

  for (; ipath != iendpath; ++ipath) {
    (*ipath)->evaluate (vpath);
  }
  for (; iprofile != iendprofile; ++iprofile) {
    (*iprofile)->evaluate (vprofile);
    (*iprofile)->evaluateTimeline (vtimeprofile);
  }
  for (; isection != iendsection; ++isection) {
    (*isection)->evaluate (vsection);
    (*isection)->evaluateNormals (vnormalsection);
  }

  std::cout << "path size = " << vpath.size () << ", section size = " << vsection.size () << "normals size = " << vnormalsection.size () << std::endl;

  for (unsigned int i = 0; i < vprofile.size () - 1; i++) {
    Vec3f y = vprofile[i];
    y.y = vtimeprofile[i];
    Vec3f y2 = vprofile[i+1];
    y2.y = vtimeprofile[i+1];
    
    float scale1 = y.x;
    float scale2 = y2.x;
    std::cout << "timeprofile = " << y.y << ", " << y2.y << std::endl;
    Vec3f pathrel1 = interpolatepath (vpath, y.y);
    Vec3f pathrel2 = interpolatepath (vpath, y2.y);

    Vec3f normale1 = interpolatepath (vnormalsection, y.y);
    Vec3f normale2 = interpolatepath (vnormalsection, y2.y);

    
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
    
    std::cout << "scale1 = " << scale1 << ", scale2 = " << scale2 << std::endl;
    std::cout << "rotate1 = " << a1 << ", rotate2 = " << a2 << std::endl;
    for (unsigned int j = 0; j < vsection.size () - 1; j++) {
      std::vector<Vec3f> face;
      std::vector<Vec3f> normal;
      face.push_back (Vec3f (pathrel1.x + vsection[j].x * scale1, 
			     pathrel1.y, 
			     vsection[j].y * scale1));
      normal.push_back (Vec3f(vnormalsection[j].x, vnormalsection[j].y,
			 vnormalsection[j].z));
      face.push_back (Vec3f (pathrel2.x + vsection[j].x * scale2, 
			     pathrel2.y, 
			     vsection[j].y * scale2));
      normal.push_back (Vec3f(vnormalsection[j].x, vnormalsection[j].y,
			vnormalsection[j].z));

      face.push_back (Vec3f (pathrel2.x + vsection[j+1].x * scale2, 
			     pathrel2.y, 
			     vsection[j+1].y * scale2));
      normal.push_back (Vec3f(vnormalsection[j+1].x, vnormalsection[j+1].y,
			vnormalsection[j+1].z));

      object.push_back (face);
      normals.push_back (normal);
      face.clear ();
      normal.clear ();
      face.push_back (Vec3f (pathrel1.x +  vsection[j].x * scale1,
			     pathrel1.y, 
			     vsection[j].y * scale1));

      normal.push_back (Vec3f(vnormalsection[j].x, vnormalsection[j].y,
			vnormalsection[j].z));


      face.push_back (Vec3f (pathrel1.x + vsection[j+1].x * scale1,
			     pathrel1.y,
			     vsection[j+1].y * scale1));
      normal.push_back (Vec3f(vnormalsection[j+1].x, vnormalsection[j+1].y,
			vnormalsection[j+1].z));


      face.push_back (Vec3f (pathrel2.x + vsection[j+1].x * scale2,
			     pathrel2.y,
			     vsection[j+1].y * scale2));
      normal.push_back (Vec3f(vnormalsection[j+1].x, vnormalsection[j+1].y,
			vnormalsection[j+1].z));
      object.push_back (face);
      normals.push_back (normal);
    }
  }
  std::cout << "END" << std::endl;
  View3DRotation::drawPolygons (object, normals);
}
