#ifndef GC_H
#define GC_H

#include <list>
#include "plugin.h"
#include "math/vector3.h"
#include "math/point.h"

class GeneralizedCylinder {
 private:
  std::list<PluginObject *> path;
  std::list<PluginObject *> profile;
  std::list<PluginObject *> section;
  float normale_scale;
  int rPath, rSection, rProfile;
 public:
  enum Curve {
    PATH,
    SECTION,
    PROFILE
  };
  GeneralizedCylinder() : normale_scale(1), rPath(10), rSection(10),
    rProfile(10) {}
  void setNormalScale (float f);
  void addPath (PluginObject *);
  void addProfile (PluginObject *);
  void addSection (PluginObject *);
  void compute ();
  void compute (std::vector<Face>&);
  void removePath (PluginObject *);
  void removeSection (PluginObject *);
  void removeProfile (PluginObject *);
  void cfgDialog ();
  void setResolution(Curve, int u);
  
  void clear () 
    { 
      path.clear (); 
      section.clear (); 
      profile.clear (); 
      m_faces.clear ();
    }

  std::vector<Face>& getFaces () {return m_faces;}

 private:
  std::vector<Face> m_faces;
};

#endif /* GC_H */
