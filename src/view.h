#ifndef VIEW_H
#define VIEW_H

#include <qmainwindow.h>
#include <qworkspace.h>
#include <qstatusbar.h>
#include "plugin.h"
#include "math/vector3.h"
#include "openglwidget.h"

class View : public QMainWindow 
{
public:
  View (QWorkspace *parent, std::string name);

  virtual void parseMousePress (QMouseEvent *e) = 0;
  virtual void parseMouseRelease (QMouseEvent *e) = 0;
  virtual void parseMouseMove (QMouseEvent *e) = 0;
  virtual void parseMouseDoubleClick (QMouseEvent *e) = 0;
  virtual void redisplay () = 0;
protected:
  void closeEvent( QCloseEvent * );
  OpenglWidget *getOpenglWidget () {return m_glwidget;}
  OpenglWidget *m_glwidget;
};

#endif /* VIEW_H */
