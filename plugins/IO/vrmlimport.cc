#include "plugin.h"

#include <vector>
#include "math/vector3.h"
#include <cstdlib>
#include <cstdio>

#define PLUGIN_NAME "VRML Import Plugin"
#define PLUGIN_MENU "Vrml Import"
#define PLUGIN_ICON "vrml.png"



namespace {
    std::vector<Vec3f> *points;
    std::vector<std::vector<int> > *faces;
};

extern "C" int yyparse ();
extern "C" void yyrestart (FILE *);

extern "C" void
addPoint(double x, double y, double z)
{
    points->push_back(Vec3f(x,y,z));
}
extern "C" void
addFace(int *vec, int len)
{
    std::vector<int> face;
    for (int i=0; i<len; i++) {
      if (vec[i]==-1) {
	faces->push_back(face);
	face.clear ();
      } else
	face.push_back(vec[i]);
    }
}

class VRMLImport : public PluginIO {
public:
  VRMLImport () : PluginIO (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON) {}
  int importData(const std::string &filename) {
    points.clear ();
    faces.clear ();
    FILE *input = fopen(filename.c_str(), "r");
    if (!input)
      return -2;
    ::points=&points;
    ::faces=&faces;
    yyrestart (input);
    yyparse();
    fclose (input);
    return 0;
  }
  const std::vector<Vec3f> &getPoints() {
    return points;
  }
  const std::vector<std::vector<int> > &getFaces() {
    return faces;
  }

private:
  std::vector<Vec3f> points;
  std::vector<std::vector<int> > faces;
};

DECLARE_PLUGIN (VRMLImport);
