#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <stdexcept>
#include <exception>
#include <qmainwindow.h>

#include <vector>

#define APPLICATION_NAME "MGModeler"
#define MAX_2DVIEWS 3

#include "view3d.h"

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow () throw ();
  
protected:
  void createMenus ();

private:
  View3D *m_view2d[MAX_2DVIEWS];
  View3D *m_view3d;

  void setViewsMode (View3D::eMode mode);
  
  signals:
    private slots:
    void menuFileOpen ();
  void menuFileQuit ();
  void menuPluginChoice ();
  void menuSelect ();
};

#endif /* MAINWINDOW_H */
