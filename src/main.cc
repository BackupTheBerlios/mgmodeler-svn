#include "mainwindow.h"
#include "plugin.h"
#include <qapplication.h>
#include <qstringlist.h>
#include <iostream>

int
main (int argc, char *argv[])
{

  QApplication application(argc, argv);
  MainWindow *mainWindow;
  
  try 
    {
      QStringList spath;
      spath+="../plugins/Objects/.libs";
      spath+="plugins/Objects/.libs";
      spath+="../plugins/IO/.libs";
      spath+="plugins/IO/.libs";
      spath+=QStringList::split (":", MODULES_DIR);
      std::cout << "searching plugins from " << spath.join(", ") << std::endl;
      PluginManager::loadPlugins (spath);
    }
  catch (std::runtime_error e)
    {
      std::cout<<"Exception during Initialization: "<<e.what ()<<std::endl;
      abort ();
    }  

  mainWindow = new MainWindow ();
  /*  mainWindow->resize (120, 300);
      mainWindow->move (10, 10);*/
  application.setMainWidget (mainWindow);

  mainWindow->show ();

  return application.exec ();
}
