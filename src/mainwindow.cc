#include "mainwindow.h"
#include "icons.h"
#include "plugin.h"

#include <qaction.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qfiledialog.h>

#include <iostream>

void
addTool (QObject *parent, const char *text, QPixmap icon, 
	 QKeySequence key, QToolBar *toolbar, QPopupMenu *menu, char *slot,
	 char *name)
{
  QAction *action = new QAction (text, icon, text, key, parent, name);
  if (menu)
    action->addTo (menu);
  if (toolbar)
    action->addTo (toolbar);
  if (slot)
    parent->connect (action, SIGNAL (activated ()), parent, slot);
}

MainWindow::MainWindow () 
  throw () 
  : QMainWindow (0, APPLICATION_NAME)
{
  mdi = new QWorkspace(this, "MDI");
  setCentralWidget(mdi);
  View2D::eView a[3] = {View2D::VIEW_PROFIL, View2D::VIEW_SECTION, 
			View2D::VIEW_PATH};


  m_view3d = new View3D (mdi);
  m_view3d-> show ();
  m_view3d->setupView ();

  for (int i=0; i<3; i++)
    {
      m_view2d[i] = new View2D (mdi, a[i%3]);
      m_view2d[i]-> updateStatusBar (0, 0);
      m_view2d[i]-> show ();
      m_view2d[i]-> setupView ();
    }
  show ();
  createMenus ();
}

void
MainWindow::createMenus ()
{
  QToolBar *toolbar = new QToolBar (this, "ToolBar");
  QPopupMenu *menu_file = new QPopupMenu (this);
  menuBar ()-> insertItem ("&File", menu_file);
  
  addTool (this, "Open File", QPixmap ((const char **)icon_fileopen), 
	   QKeySequence ("Ctrl+O"), toolbar, menu_file, SLOT(menuFileOpen()),
	   NULL);
  menu_file->insertSeparator();

  addTool (this, "Quit",((const char **)icon_exit) ,
	   QKeySequence ("Ctrl+Q"), NULL, menu_file, SLOT(menuFileQuit()),
	   NULL);


  addTool (this, "Selection Mode", ((const char **)icon_cursor),
	   QKeySequence ("Ctrl+S"), toolbar, NULL, 
	   SLOT(menuSelect()), NULL);


  std::vector<Plugin *>::iterator i;
  int k = 0;

  for (i = PluginManager::iobegin (); i != PluginManager::ioend (); ++i, ++k)
    {
      char str[2] = "0";
      str[0] = k;
      Plugin *p = *i;
      addTool (this, p->getName ().c_str (), QPixmap(p-> getIcon ()),
	       QKeySequence (), NULL, menu_file, SLOT(menuPluginIOChoice()),
	       str);
    }
  k=0;
  std::cout <<"PLUGINS COUNT = "<<PluginManager::pluginsCount ()<<"\n";
  
  for (i = PluginManager::begin (); i != PluginManager::end (); ++i, ++k)
    {
      char str[2] = "0";
      str[0] = k;
      Plugin *p = *i;
      addTool (this, p->getName ().c_str (), QPixmap(p-> getIcon ()),
	       QKeySequence (), toolbar, NULL, 
	       SLOT(menuPluginChoice()), str);
    }


  QPopupMenu *menu_window = new QPopupMenu (this);
  menuBar ()-> insertItem ("&Windows", menu_window);
  int cascadeId = menu_window->insertItem("&Cascade", mdi, SLOT(cascade() ) );
  int tileId = menu_window->insertItem("&Tile", mdi, SLOT(tile() ) );
  mdi->tile ();
  mdi->setScrollBarsEnabled(true);
  if ( mdi->windowList().isEmpty() ) {
    menu_window->setItemEnabled( cascadeId, FALSE );
    menu_window->setItemEnabled( tileId, FALSE );
  }
  menu_window->insertSeparator();
  QWidgetList windows = mdi->windowList();
  for ( int i = 0; i < int(windows.count()); ++i ) {
    int id = menu_window->insertItem(windows.at(i)->caption(),
				     this, SLOT( windowsMenuActivated( int ) ) );
    menu_window->setItemParameter( id, i );
    /*    menu_window->setItemChecked( id, mdi->activeWindow() == windows.at(i) );*/
  }  
}


void MainWindow::setViewsMode (View2D::eMode mode)
{
  for (int i=0; i<3; i++)
    {
      m_view2d[i]-> setMode (mode);
    }
}

void
MainWindow::menuFileOpen ()
{
  QString s = QFileDialog::getOpenFileName(
                    "./",
                    "csg (*.csg)",
                    this,
                    "open file dialog",
                    "Choose a file to open" );
}


void
MainWindow::menuFileQuit ()
{
  close ();
}

void
MainWindow::menuPluginChoice ()
{
  PluginObject *objp = (PluginObject *)PluginManager::getPlugin 
    (sender ()-> name  ()[0]);
  
  //std::cout<<"Plugin: "<< objp->getName ()<<"\n";

  View2D::setCurrentPlugin (objp);
  setViewsMode (View2D::MODE_EDIT);
}

void
MainWindow::menuPluginIOChoice ()
{
  PluginIO *objp = (PluginIO *)PluginManager::getIOPlugin 
    (sender ()->name()[0]);

  QString s = QFileDialog::getOpenFileName(
		    "./",
                    "csg (*.wrl)",
                    this,
                    "open file dialog",
                    "Choose a file to open" );


  objp->parse(s);
  static_cast<View3D *>(m_view3d)->current=objp;
}

void
MainWindow::windowsMenuActivated(int id)
{
   QWidget* w = mdi->windowList().at( id );
   if ( w )
     w->showNormal();
   w->setFocus();
}
void
MainWindow::menuSelect ()
{
  std::cout<<"SELECT\n";
  setViewsMode (View2D::MODE_SELECTION);
}
