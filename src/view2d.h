#ifndef VIEW2D_H
#define VIEW2D_H

#include "view.h"
#include "plugin.h"

class View2D : public View
{
public:
  enum eMode {
    MODE_EDIT,
    MODE_SELECTION,
    MODE_MOVE_WINDOW,
    MODE_SELECTION_OBJECT
  };

  enum eView {
    VIEW_PROFIL,
    VIEW_SECTION,
    VIEW_PATH
  };

  View2D (QWorkspace *parent, eView view);

  void setupView ();
  void updateStatusBar (float x, float y);

  void parseMousePress (QMouseEvent *e);
  void parseMouseRelease (QMouseEvent *e);
  void parseMouseMove (QMouseEvent *e);
  void parseMouseDoubleClick (QMouseEvent *e);
  void redisplay ();
  static void setCurrentPlugin (PluginObject *p);
  const std::vector<PluginObject *>& getPlugins();

  PluginObject *getActivePlugin () { return m_plugin_active; }

  void setMode (eMode mode) {m_mode = mode;}
  void updateStatusBar (const Vec3f& v);
 private:
  void beginWindowMotion (int x, int y);
  void parseWindowMotion (int x, int y);
  void endWindowMotion ();
  static PluginObject *s_plugin_current;
  PluginObject *m_plugin_active;
  eView m_view;
  bool m_mouse_move;
  int m_mouse_move_x, m_mouse_move_y;
  int m_cursor_x, m_cursor_y;
  eMode m_mode;
  std::vector<PluginObject *> m_plugins;
};

#endif /* ! VIEW2D_H */


