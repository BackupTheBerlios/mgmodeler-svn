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
  eView a[3] = {VIEW_PROFIL, VIEW_SECTION, VIEW_WAY};


  for (int i=0; i<3; i++)
    {
      m_view2d[i] = new View3D (NULL, a[i%3]);
      m_view2d[i]-> resize (500, 400);
      m_view2d[i]-> updateStatusBar (0, 0);
      m_view2d[i]-> move (150+(i%2)*520, 10 + (i/2) * 440);
      m_view2d[i]-> show ();
      m_view2d[i]-> setupView ();
    }

  m_view3d = new View3DRotation (NULL);
  m_view3d-> resize (500, 400);
  m_view3d-> show ();
  m_view3d->move (670, 450);
  m_view3d->setupView ();

  createMenus ();
}

void
MainWindow::createMenus ()
{
  QToolBar *toolbar = new QToolBar (this, "ToolBar");
  QPopupMenu *menu_file = new QPopupMenu (this);
  menuBar ()-> insertItem ("File", menu_file);
  
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
}


void MainWindow::setViewsMode (View3D::eMode mode)
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

  m_view3d-> setCurrentPlugin (objp);
  setViewsMode (View3D::MODE_EDIT);
}

void
MainWindow::menuSelect ()
{
  std::cout<<"SELECT\n";
  setViewsMode (View3D::MODE_SELECTION);
}
