#ifndef GC_H
#define GC_H

#include <list>
#include "plugin.h"

class GeneralizedCylinder {
 private:
  std::list<PluginObject *> path;
  std::list<PluginObject *> profile;
  std::list<PluginObject *> section;
 public:
  void addPath (PluginObject *);
  void addProfile (PluginObject *);
  void addSection (PluginObject *);
  void display ();
  void removePath (PluginObject *);
  void removeSection (PluginObject *);
  void removeProfile (PluginObject *);
  void cfgDialog ();
};

#endif /* GC_H */
