#include "view.h"
#include "openglwidget.h"

#include <qgl.h>
#include <qmainwindow.h>
#include <string>

View::View (QWorkspace *parent, std::string name)
  :QMainWindow (parent, name.c_str (), 0)
{
  m_glwidget = new OpenglWidget (this, name.c_str (), true, false);
  setCentralWidget (m_glwidget);
}

void
View::closeEvent( QCloseEvent *e )
{
  std::cout << "refusing closing event" << std::endl;
  e->ignore();
}
