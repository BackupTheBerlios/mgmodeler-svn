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
 public:
  void addPath (PluginObject *);
  void addProfile (PluginObject *);
  void addSection (PluginObject *);
  void compute (std::vector<Face>&);
  void removePath (PluginObject *);
  void removeSection (PluginObject *);
  void removeProfile (PluginObject *);
  void cfgDialog ();
};

#endif /* GC_H */
