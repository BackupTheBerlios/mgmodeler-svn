#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <qgl.h>

class OpenglWidget : public QGLWidget
{
public:
  OpenglWidget (QWidget *parent, const char *name);

  void initializeGL ();
  void clearGL ();
  void resizeGL (int w, int h);
  void paintGL ();

  void setOrthoView (bool flag) {m_orthoview = flag;}

private:
  void mousePressEvent (QMouseEvent *e);
  void mouseMoveEvent (QMouseEvent *e);

  int m_width;
  int m_height;

  QWidget *m_parent;
  bool m_orthoview;

};

#endif
