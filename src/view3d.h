#ifndef VIEW3D_H
#define VIEW3D_H

#include <qmainwindow.h>
#include <qworkspace.h>
#include <qstatusbar.h>

class OpenglWidget;
class Trackball;


typedef enum {VIEW_PROFIL, VIEW_SECTION, VIEW_WAY} eView;

class View3D : public QMainWindow 
{
public:
  View3D (QWorkspace *parent, std::string name);
  View3D (QWorkspace *parent, eView view);

  virtual void setupView ();
  virtual void updateStatusBar (int x, int y);

  virtual void parseMousePress (QMouseEvent *e);
  virtual void parseMouseMove (QMouseEvent *e);

protected:

  eView m_view;
  OpenglWidget *m_glwidget;

  int m_cursor_x, m_cursor_y;
  bool m_editable;
};


class View3DRotation : public View3D {
public:
  View3DRotation (QWorkspace *parent);

  void setupView ();
  void updateStatusBar (int x, int y);


  void parseMousePress (QMouseEvent *e);
  void parseMouseMove (QMouseEvent *e);

private:
};

#endif


