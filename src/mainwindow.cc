#include "mainwindow.h"
#include "plugin.h"
#include <qaction.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>

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
  m_view2d_type[0] = View2D::VIEW_PROFIL;
  m_view2d_type[1] = View2D::VIEW_SECTION;
  m_view2d_type[2] = View2D::VIEW_PATH;


  m_subconfig = new SubConfig (this, "Subdivision Config", false, 
			       Qt::WStyle_Customize | Qt::WStyle_Tool |
			       Qt::WStyle_DialogBorder);

  m_view3d = new View3D (mdi);
  m_view3d-> show ();
  m_view3d-> setupView ();

  for (int i=0; i<3; i++)
    {
      m_view2d[i] = new View2D (mdi, m_view2d_type[i%3], this);
      m_view2d[i]-> updateStatusBar (0, 0);
      m_view2d[i]-> show ();
      m_view2d[i]-> setupView ();
    }
}

void
MainWindow::show ()
{
  setWindowState(WindowMaximized);
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

  addTool (this, "New File", QPixmap::fromMimeSource ("filenew.png"), 
	   QKeySequence ("Ctrl+N"), toolbar, menu_file, SLOT(menuFileNew()),
	   NULL);
  
  addTool (this, "Open File", QPixmap::fromMimeSource ("fileopen.png"), 
	   QKeySequence ("Ctrl+O"), toolbar, menu_file, SLOT(menuFileOpen()),
	   NULL);

  addTool (this, "Save File", QPixmap::fromMimeSource ("filesave.png"), 
	   QKeySequence ("Ctrl+S"), toolbar, menu_file, SLOT(menuFileSave()),
	   NULL);

  addTool (this, "Close Import",QPixmap::fromMimeSource ("fileclose.png") ,
	   QKeySequence ("Ctrl+Q"), toolbar, menu_file, SLOT(menuFileClose()),
	   NULL);

  menu_file->insertSeparator();

  addTool (this, "Quit",QPixmap::fromMimeSource ("exit.png") ,
	   QKeySequence ("Ctrl+Q"), NULL, menu_file, SLOT(menuFileQuit()),
	   NULL);

  addTool (this, "Select Object", QPixmap::fromMimeSource ("select.png"),
	   QKeySequence ("Ctrl+S"), toolbar, NULL, SLOT(menuSelect ()),
	   NULL);

  addTool (this, "Move 2d Window", QPixmap::fromMimeSource ("move.png"),
	   QKeySequence ("Ctrl+M"), toolbar, NULL, SLOT(menuWindowMove ()),
	   NULL);

  toolbar->addSeparator();  
 
  for (i = PluginManager::iobegin (); i != PluginManager::ioend (); ++i, ++k)
    {
      char str[2] = "0";
      str[0] = k;
      Plugin *p = *i;
      QPopupMenu *pg_target_menu = NULL;
      
      if (p->getType () == Plugin::PLUGIN_IO_IMPORT)
	pg_target_menu = menu_pg_import;

      if (p->getType () == Plugin::PLUGIN_IO_EXPORT)
	pg_target_menu = menu_pg_export;

      addTool (this, p->getMenu ().c_str (), 
	       QPixmap (p-> getIcon ()),
	       QKeySequence (), NULL, pg_target_menu, 
	       SLOT(menuPluginIOChoice()), str);
    }

  for (i = PluginManager::begin (), k=0; i != PluginManager::end (); ++i, ++k)
    {
      char str[2] = "0";
      str[0] = k;
      Plugin *p = *i;
      addTool (this, p->getMenu ().c_str (), 
	       QPixmap (p-> getIcon ()),
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
  }  

  addTool (this, "Delete Object", QPixmap::fromMimeSource ("delete.png"),
	   QKeySequence ("Ctrl+D"), toolbar, menu_plugins, 
	   SLOT(menuDeleteObject ()), NULL);

  toolbar->addSeparator(); 

  addTool (this, "Wireframe", QPixmap::fromMimeSource ("wireframe.png"),
	   QKeySequence ("Ctrl+W"), toolbar, NULL, SLOT(menuWireframe ()),
	   NULL);

  addTool (this, "Fill", QPixmap::fromMimeSource ("fill.png"),
	   QKeySequence ("Ctrl+F"), toolbar, NULL, SLOT(menuFill ()),
	   NULL);

  addTool (this, "Lighting", QPixmap::fromMimeSource ("light.png"),
	   QKeySequence ("Ctrl+L"), toolbar, NULL, SLOT(menuLighting ()),
	   NULL);

  addTool (this, "Normals", QPixmap::fromMimeSource ("normals.png"),
	   QKeySequence ("Ctrl+N"), toolbar, NULL, SLOT(menuNormals ()),
	   NULL);

  addTool (this, "Switch Normals", QPixmap::fromMimeSource ("snormals.png"),
	   QKeySequence ("Ctrl+S"), toolbar, NULL, SLOT(menuSwitchNormals ()),
	   NULL);

  toolbar->addSeparator(); 

  addTool (this, "Configure", QPixmap::fromMimeSource ("configure.png"), 
	   QKeySequence ("Ctrl-C"), toolbar, menu_plugins, SLOT(menuConfig()),
	   NULL);

  toolbar->addSeparator(); 

  addTool (this, "Compute Generalized Cylinder", 
	   QPixmap::fromMimeSource ("compute.png"), 
	   QKeySequence ("Ctrl-R"), toolbar, NULL, SLOT(menuCompute()),
	   NULL);

  toolbar->addSeparator(); 

  addTool (this, "Manual", QPixmap::fromMimeSource ("help.png"), 
	   QKeySequence ("F1"), toolbar, menu_help, SLOT(menuHelp()),
	   NULL);
}


void MainWindow::setViewsMode (View2D::eMode mode)
{
  for (int i=0; i<3; i++)
    m_view2d[i]-> setMode (mode);
}

void
MainWindow::menuFileNew ()
{
  View::getGC ().clear ();
  for (int i=0; i<3; i++)
    {
      m_view2d[i]-> Init ();
      m_view2d[i]-> display ();
    }
  m_view3d->display ();
}

void
MainWindow::menuFileOpen ()
{
  QString s = QFileDialog::getOpenFileName(
                    "./",
                    "gc (*.gc)",
                    this,
                    "open file dialog",
                    "Choose a file to open" );
  std::ifstream stream (s);
  std::string type;
  
  for (int i=0; i<3; i++) {
    stream >> type;
    std::cout << "*" << type << std::endl;
    if (type == "PROFILE") {
      m_view2d[0]->load (stream);
    } else {
      if (type == "SECTION") {
	m_view2d[1]->load (stream);
      } else {
	if (type == "PATH") {
	  m_view2d[2]->load (stream);
	}
      }
    }
  }
}


void
MainWindow::menuFileSave ()
{
  QString s = QFileDialog::getSaveFileName(
                    "./",
                    "gc (*.gc)",
                    this,
                    "save file dialog",
                    "Choose a file to save" );

  
  std::ofstream stream (s);
  for (int i=0; i<3; i++) {
    switch (m_view2d_type[i]) {
    case View2D::VIEW_PROFIL:
      stream << "PROFILE" << std::endl;
      break;
    case View2D::VIEW_SECTION:
      stream << "SECTION" << std::endl;
      break;
    case View2D::VIEW_PATH:
      stream << "PATH" << std::endl;
    }
    m_view2d[i]->save (stream);
  }
}

void
MainWindow::menuFileClose ()
{
  m_view3d->Init ();
  m_view3d->display ();
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
MainWindow::menuDeleteObject ()
{
  setViewsMode (View2D::MODE_DELETE_OBJECT);
}


void
MainWindow::checkIOError (const std::string& filename, const int result)
  throw (std::runtime_error)
{
  switch (result)
    {
    case 0: /* No Error */   break;
    case -1: throw std::runtime_error ("FATAL: Function Not Implemented");
    case -2: QMessageBox::
      critical (this, "Error", std::string("Error opening file ")+filename);
    }
}

void
MainWindow::menuPluginIOChoice ()
{
  PluginIO *objp = (PluginIO *)PluginManager::getIOPlugin 
    (sender ()->name()[0]);

  QString s;
  std::vector<Face> f;

  switch (objp->getType ())
    {
    case Plugin::PLUGIN_IO_IMPORT:
      s = QFileDialog::getOpenFileName ( "./", "wrl (*.wrl)", 
			  this, "open file dialog", "Choose a file to open" );

      if (!s.isEmpty ())
	{
	  int ret = objp->importData(s);
	  if (!ret)
	    static_cast<View3D *>(m_view3d)->current=objp;
	  else
	    checkIOError (s, ret);
	}
      break;
    case Plugin::PLUGIN_IO_EXPORT:
      f = m_view3d->getFaces ();
      if (!f.size ())
	{
	  QMessageBox::
	    critical 
	    (this, "Error", 
	     std::string 
	     ("You must compute a generalized cylinder before saving"));
	  return ;
	}

      s = QFileDialog::getSaveFileName ( "./", "wrl (*.wrl)", 
					 this, "open file dialog", 
					 "Choose a file for writting" );

      if (!s.isEmpty ())
	{
	  int ret = objp->exportData (s, f); 
	  if (ret)
	    checkIOError (s, ret);
	}
      break;
    default:
      break;
    }
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
  char path[256];
  sprintf (path, "cd %s && assistant assistant -profile index.dcf&", DATADIR);
  system (path);
}

void
MainWindow::menuLighting ()
{
  m_view3d-> setLighting ();
}

void
MainWindow::menuFill ()
{
  m_view3d-> setFillMode ();
}

void
MainWindow::menuWireframe ()
{
  m_view3d-> setWireframe ();
}


void
MainWindow::menuNormals ()
{
  m_view3d-> toggleNormals ();
}

void
MainWindow::menuSwitchNormals ()
{
  m_view3d-> switchNormals ();
}

void
MainWindow::menuConfig ()
{
  m_subconfig->show ();  
}

void
MainWindow::view3dRedisplay ()
{
  m_view3d->redisplay ();
}

void
MainWindow::menuSelect ()
{
  setViewsMode (View2D::MODE_SELECTION_OBJECT);
}


void
MainWindow::menuCompute ()
{
  View::getGC ().compute ();
  m_view3d->display ();
}
