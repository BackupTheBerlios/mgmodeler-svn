#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdexcept>
#include <exception>
#include <qmainwindow.h>

#include <vector>

#define APPLICATION_NAME "MGModeler"
#define MAX_2DVIEWS 3

#include "view2d.h"
#include "view3d.h"
#include "subconfig.h"

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow () throw ();
  void show ();
  void setViewsMode (View2D::eMode mode);
protected:
  void createMenus ();

private:
  QWorkspace *mdi;
  View2D *m_view2d[MAX_2DVIEWS];
  View3D *m_view3d;

  SubConfig *m_subconfig;

 
  signals:
  public slots:
    void view3dRedisplay ();
  private slots:
    
    void menuFileOpen ();
  void menuFileQuit ();
  void menuPluginChoice ();
  void menuPluginIOChoice ();
  void menuWindowMove ();
  void menuHelp ();
  void menuLighting ();
  void menuFill ();
  void menuWireframe ();
  void menuNormals ();
  void menuSwitchNormals ();
  void menuConfig ();
  void menuSelect ();
  void windowsMenuActivated(int id);

  friend class View2D;
};

#endif /* MAINWINDOW_H */
