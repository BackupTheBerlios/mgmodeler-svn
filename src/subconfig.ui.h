/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include "view3d.h"
#include "mainwindow.h"
#include "gc.h"

void SubConfig::SubPath_valueChanged( int u)
{
    MainWindow *p = dynamic_cast<MainWindow *>(parent());
    View::gc.setResolution(GeneralizedCylinder::PATH,  u);
    p->view3dRedisplay ();
}


void SubConfig::SubSection_valueChanged( int u)
{
    MainWindow *p = dynamic_cast<MainWindow *>(parent());
    View::gc.setResolution(GeneralizedCylinder::SECTION,  u);
    p->view3dRedisplay ();
}


void SubConfig::SubProfil_valueChanged( int u)
{
    MainWindow *p = dynamic_cast<MainWindow *>(parent());
    View::gc.setResolution(GeneralizedCylinder::PROFILE,  u);
    p->view3dRedisplay ();
}
