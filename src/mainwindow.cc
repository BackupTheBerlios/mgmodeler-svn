#include "mainwindow.h"
#include "view3d.h"

#include <qaction.h>
#include <qpopupmenu.h>
#include <qmenubar.h>

#include <iostream>

void
addTool (QObject *parent, char *text, QPixmap icon, 
	 QKeySequence key, QToolBar *toolbar, QPopupMenu *menu,
	 char *slot)
{
  QAction *action = new QAction (text, icon, text, key, parent, text);
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
  eAxe a[3] = {AXE_X, AXE_Y, AXE_Z};

  for (int i=0; i<MAX_2DVIEWS; i++)
    {
      m_view2d[i] = new View3D (NULL, a[i%3]);
      m_view2d[i]-> resize (500, 400);
      m_view2d[i]-> updateStatusBar (0, 0);
      m_view2d[i]-> show ();
      m_view2d[i]-> setupView ();
    }

  m_view3d = new View3DRotation (NULL);
  m_view3d-> resize (500, 400);
  m_view3d-> show ();
  m_view3d->setupView ();

  createMenus ();
}

void
MainWindow::createMenus ()
{
  QToolBar *toolbar = new QToolBar (this, "ToolBar");
  QPopupMenu *menu_file = new QPopupMenu (this);
  menuBar ()-> insertItem ("File", menu_file);
  
  addTool (this, "Open File", NULL, QKeySequence ("Ctrl+O"), 
	   toolbar, menu_file, SLOT(menuFileOpen()));
  menu_file->insertSeparator();
  addTool (this, "Quit", NULL, QKeySequence ("Ctrl+Q"), 
	   toolbar, menu_file, SLOT(menuFileQuit()));
}

void
MainWindow::menuFileOpen ()
{
}


void
MainWindow::menuFileQuit ()
{
  close ();
}
