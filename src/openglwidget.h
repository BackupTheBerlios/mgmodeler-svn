#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <qgl.h>

class Trackball;

class OpenglWidget : public QGLWidget
{
public:
  OpenglWidget (QWidget *parent, const char *name, bool ortho, bool trackball);

  void initializeGL ();
  void clearGL ();
  void resizeGL (int w, int h);
  void paintGL ();

  void setOrthoView (bool flag) {m_orthoview = flag;}

private:
  void mousePressEvent (QMouseEvent *e);
  void mouseMoveEvent (QMouseEvent *e);
  void mouseReleaseEvent (QMouseEvent *e);
  void wheelEvent (QWheelEvent * e);

  void SyncContext ();

  int m_width;
  int m_height;

  QWidget *m_parent;
  bool m_orthoview;

  bool m_update_modelview;
  bool m_update_projection;
  
  Trackball *m_trackball;
  float m_trackball_zoom;
  bool m_trackball_enable;
  static const float TRACKBALL_ZOOM_DEF = 0.5;

};

#endif
