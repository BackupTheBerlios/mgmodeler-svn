#include "mainwindow.h"
#include "plugin.h"
#include <qassistantclient.h>
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


  m_subconfig = new SubConfig (this, "Subdivision Config", true);

  m_view3d = new View3D (mdi);
  m_view3d-> show ();
  m_view3d-> setupView ();

  for (int i=0; i<3; i++)
    {
      m_view2d[i] = new View2D (mdi, a[i%3]);
      m_view2d[i]-> updateStatusBar (0, 0);
      m_view2d[i]-> show ();
      m_view2d[i]-> setupView ();
    }
}

void
MainWindow::show ()
{
  QMainWindow::show();
  createMenus ();
}

void
MainWindow::createMenus ()
{
  std::vector<Plugin *>::iterator i;
  int k = 0;

  QToolBar *toolbar = new QToolBar (this, "ToolBar");
  QPopupMenu *menu_file = new QPopupMenu (this);
  QPopupMenu *menu_help = new QPopupMenu (this);
  QPopupMenu *menu_window = new QPopupMenu (this);
  QPopupMenu *menu_plugins = new QPopupMenu (this);
  QPopupMenu *menu_pg_objects = new QPopupMenu (this);
  QPopupMenu *menu_pg_import = new QPopupMenu (this);
  QPopupMenu *menu_pg_export = new QPopupMenu (this);

  menuBar ()-> insertItem ("&File", menu_file);
  menuBar ()-> insertItem ("&Plugins", menu_plugins);
  menuBar ()-> insertItem ("&Windows", menu_window);
  menuBar ()-> insertItem ("&Help", menu_help);
  menu_plugins-> insertItem ("&Objects", menu_pg_objects);
  menu_plugins-> insertItem ("&Import", menu_pg_import);
  menu_plugins-> insertItem ("&Export", menu_pg_export);
  
  addTool (this, "Open File", QPixmap ("icons/fileopen.png"), 
	   QKeySequence ("Ctrl+O"), toolbar, menu_file, SLOT(menuFileOpen()),
	   NULL);

  menu_file->insertSeparator();

  addTool (this, "Quit",QPixmap ("icons/exit.png") ,
	   QKeySequence ("Ctrl+Q"), NULL, menu_file, SLOT(menuFileQuit()),
	   NULL);

  addTool (this, "Move 2d Window", QPixmap ("icons/move.png"),
	   QKeySequence ("Ctrl+M"), toolbar, NULL, SLOT(menuWindowMove ()),
	   NULL);

  toolbar->addSeparator();  
 
  for (i = PluginManager::iobegin (); i != PluginManager::ioend (); ++i, ++k)
    {
      char str[2] = "0";
      str[0] = k;
      Plugin *p = *i;
      QString menu_name = QString (p-> getMenu ()).section ('/',0,0);
      QString menu_target = QString (p->getMenu ()).section('/',1,1);
      QPopupMenu *pg_target_menu = NULL;
      
      if (menu_name == QString ("import"))
	pg_target_menu = menu_pg_import;

      if (menu_name == QString ("export"))
	pg_target_menu = menu_pg_export;

      addTool (this, p->getName ().c_str (), QPixmap(p-> getIcon ()),
	       QKeySequence (), NULL, pg_target_menu, 
	       SLOT(menuPluginIOChoice()), str);
    }

  for (i = PluginManager::begin (), k=0; i != PluginManager::end (); ++i, ++k)
    {
      char str[2] = "0";
      str[0] = k;
      Plugin *p = *i;
      addTool (this, p->getMenu ().c_str (), QPixmap(p-> getIcon ()),
	       QKeySequence (), toolbar, menu_pg_objects,
	       SLOT(menuPluginChoice()), str);
    }


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
    int id = 
      menu_window->insertItem(windows.at(i)->caption(),
			      this, SLOT( windowsMenuActivated( int ) ) );
    menu_window->setItemParameter( id, i );
    /*    menu_window->setItemChecked
	  ( id, mdi->activeWindow() == windows.at(i) );*/
  }  

  toolbar->addSeparator(); 

  addTool (this, "Wireframe", QPixmap ("icons/wireframe.png"),
	   QKeySequence ("Ctrl+W"), toolbar, NULL, SLOT(menuWireframe ()),
	   NULL);

  addTool (this, "Fill", QPixmap ("icons/fill.png"),
	   QKeySequence ("Ctrl+F"), toolbar, NULL, SLOT(menuFill ()),
	   NULL);

  addTool (this, "Lighting", QPixmap ("icons/light.png"),
	   QKeySequence ("Ctrl+L"), toolbar, NULL, SLOT(menuLighting ()),
	   NULL);

  addTool (this, "Normals", QPixmap ("icons/normals.png"),
	   QKeySequence ("Ctrl+N"), toolbar, NULL, SLOT(menuNormals ()),
	   NULL);

  addTool (this, "Switch Normals", QPixmap ("icons/snormals.png"),
	   QKeySequence ("Ctrl+S"), toolbar, NULL, SLOT(menuSwitchNormals ()),
	   NULL);

  toolbar->addSeparator(); 

  addTool (this, "Configure", QPixmap ("icons/configure.png"), 
	   QKeySequence ("Ctrl-C"), toolbar, menu_plugins, SLOT(menuConfig()),
	   NULL);

  toolbar->addSeparator(); 

  addTool (this, "Manual", QPixmap ("icons/help.png"), 
	   QKeySequence ("F1"), toolbar, menu_help, SLOT(menuHelp()),
	   NULL);
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
  
  View2D::setCurrentPlugin (objp);
  setViewsMode (View2D::MODE_EDIT);
}

void
MainWindow::menuPluginIOChoice ()
{
  PluginIO *objp = (PluginIO *)PluginManager::getIOPlugin 
    (sender ()->name()[0]);

  QString s = QFileDialog::getOpenFileName
    ( "./", "csg (*.wrl)", this, "open file dialog", "Choose a file to open" );

  if (s.isEmpty ())
    return;

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
MainWindow::menuWindowMove ()
{
  setViewsMode (View2D::MODE_MOVE_WINDOW);
}

void
MainWindow::menuHelp ()
{
  /*    QAssistantClient *helpclient = 
	new QAssistantClient (QDir("help/").absPath(), this );*/
  system ("assistant -profile help/index.dcf&");
  /*QString docsPath = QDir("help/").absPath();
    helpclient->showPage( QString("%1/index.dcf").arg(docsPath) );*/
}

void
MainWindow::menuLighting ()
{
  m_view3d-> setLighting ();
  m_view3d->redisplay ();
}

void
MainWindow::menuFill ()
{
  m_view3d-> setFillMode ();
  m_view3d->redisplay ();
}

void
MainWindow::menuWireframe ()
{
  m_view3d-> setWireframe ();
  m_view3d->redisplay ();
}


void
MainWindow::menuNormals ()
{
  m_view3d-> toggleNormals ();
  m_view3d->redisplay ();
}

void
MainWindow::menuSwitchNormals ()
{
  m_view3d-> switchNormals ();
  m_view3d->redisplay ();
}

void
MainWindow::menuConfig ()
{
  m_subconfig->show ();
}
