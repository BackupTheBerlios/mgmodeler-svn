#ifndef VIEW3D_H
#define VIEW3D_H

#include <qworkspace.h>
#include "view.h"
#include "gc.h"

class View3D : public View
{
public:
  View3D (QWorkspace *parent, const std::string& name = "View3D");

  void Init () {current = NULL;}

  void setupView ();
  void updateStatusBar (const std::string& s);

  void parseMousePress (QMouseEvent *e);
  void parseMouseRelease (QMouseEvent *e);
  void parseMouseMove (QMouseEvent *e);
  void parseMouseDoubleClick (QMouseEvent *e);
  
  void setCurrentPlugin (PluginIO *p);
  PluginIO *getCurrentPlugin () { return current; }
  
  std::vector<Face> getFaces () 
    {
      std::vector<Face> f;
      f = gc.getFaces ();
      return f;
    }
  
  void redisplay ();

  void setLighting ();
  void setFillMode ();
  void setWireframe ();
  void toggleNormals ();
  void switchNormals ();

  /* FIXME */
public:
  PluginIO *current;
  int normale_scale;
};

#endif


