#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <vector>
#include <qgl.h>
#include "math/vector3.h"
#include "trackball.h"
#include "math/point.h"

/* FIXME */
#define Vec2f Vec3f

class View;

class OpenglWidget : public QGLWidget
{
public:
  OpenglWidget (QWidget *parent, const char *name, bool ortho, bool trackball);

  void initializeGL ();
  void clearGL ();
  void resizeGL (int w, int h);
  void paintGL ();

  void setOrthoView (bool flag) {m_orthoview = flag;}
  void setTrackBall (bool flag) {
    m_trackball_enable = flag;
    if (m_trackball_enable)
      {
	m_trackball = new Trackball (100, 100);
	m_trackball_zoom = TRACKBALL_ZOOM_DEF;
	m_update_modelview = true;
      } else {
	delete m_trackball;
	m_trackball=0;
	m_update_modelview = false;
      }
  }
  static void unProject (const Vec2f& i, Vec3f& o);

  void drawPolygons (const std::vector<Vec3f>& points,
		     const std::vector<std::vector<int> >& faces);
  void drawPolygons (const std::vector<std::vector<Vec3f> >& faces,
		     const std::vector<std::vector<Vec3f> >& normals);
  void drawPolygons (const std::vector<Face>& faces);

private:
  void mousePressEvent (QMouseEvent *e);
  void mouseMoveEvent (QMouseEvent *e);
  void mouseReleaseEvent (QMouseEvent *e);
  void wheelEvent (QWheelEvent * e);
  void mouseDoubleClickEvent (QMouseEvent *e);


  void SyncContext ();

  int m_width;
  int m_height;

  View *m_parent;
  bool m_orthoview;

  bool m_update_modelview;
  bool m_update_projection;
  
  Trackball *m_trackball;
  float m_trackball_zoom;
  bool m_trackball_enable;
  static const float TRACKBALL_ZOOM_DEF = 0.5;
};

#endif
