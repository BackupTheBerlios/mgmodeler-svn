#include "openglwidget.h"
#include "view3d.h"
#include "trackball.h"
#include "gc.h"
#include <qgl.h>
#include <iostream>

OpenglWidget::OpenglWidget (QWidget *parent, const char *name, 
			    bool ortho, bool trackball)
  :QGLWidget (parent, name), m_trackball_zoom (TRACKBALL_ZOOM_DEF)
{
  m_parent = static_cast<View *>(parent);
  m_orthoview = ortho;
  m_update_projection = true;
  setMouseTracking (true);
  m_trackball_enable = trackball;
  m_lighting = false;
  m_wireframe = false;
  m_normals = false;
  if (m_trackball_enable)
    {
      m_trackball = new Trackball (100, 100);
      m_trackball_zoom = TRACKBALL_ZOOM_DEF;
      m_update_modelview = true;
    } else 
      m_trackball = 0;

  m_motion_x = m_motion_y = 0.0;
}

void 
OpenglWidget::initializeGL ()
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glMatrixMode (GL_MODELVIEW);
  glEnable (GL_DEPTH_TEST);
}

void 
OpenglWidget::clearGL ()
{
  double size = 5.0;

  if (m_orthoview)
    glClearColor (1.0, 1.0, 1.0, 1.0);
  else
    glClearColor (0.0, 0.0, 0.0, 1.0);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_orthoview)
    {
      glBegin (GL_LINES);

      glColor3f (0.0, 1.0, 0.0);
      glVertex2f (0.0, 0.0);
      glVertex2f (size, 0.0);
      glVertex2f (0.0, 0.0);
      glVertex2f (0.0, size);
      glColor3f (1.0, 0.0, 0.0);
      glVertex2f (0.0, 0.0);
      glVertex2f (-size, 0.0);
      glVertex2f (0.0, 0.0);
      glVertex2f (0.0, -size);

      glColor3f (.85, .85, .85);
      for (double i=-size; i<size; i+=0.1)
	if (i!= 0)
	  {
	    glVertex2f (i, -size);
	    glVertex2f (i, size);
	    glVertex2f (-size, i);
	    glVertex2f (size, i);
	  }
	  
      glEnd ();
    }
  
}

void 
OpenglWidget::resizeGL (int w, int h)
{
  if (m_trackball_enable)
    m_trackball->setScreenSize (w, h);

  m_width = w;
  m_height = h;

  m_update_modelview = true;
  m_update_projection = true;


  updateGL ();
}

void 
OpenglWidget::paintGL ()
{
  SyncContext ();  
  clearGL ();
  m_parent->redisplay (); 
  swapBuffers ();
}


void
OpenglWidget::mousePressEvent (QMouseEvent *e) {
  m_parent-> parseMousePress (e);

  switch (e->button ()) {
  case QMouseEvent::LeftButton:
    if (m_trackball_enable)
      {
	m_trackball->startRotation (e->x (), e-> y());
	m_update_modelview = true;
      }
    break;
  default:
    break;
  }
  updateGL ();
}

void
OpenglWidget::mouseDoubleClickEvent (QMouseEvent *e)
{
  m_parent->parseMouseDoubleClick (e);
  updateGL ();
}

void
OpenglWidget::mouseReleaseEvent (QMouseEvent *e) {
  m_parent->parseMouseRelease (e);

  switch (e->button ()) {
  case QMouseEvent::LeftButton:
    if (m_trackball_enable)
      {
	m_trackball->stopRotation ();
	m_update_modelview = true;
      }
    break;
  default:
    break;
  }
  
  updateGL ();
}


void
OpenglWidget::mouseMoveEvent (QMouseEvent *e) {
  m_parent-> parseMouseMove (e);
  
  if (e-> state ()&QMouseEvent::LeftButton)
    if (m_trackball_enable)
      {
	m_trackball->computeOrientation (e->x (), e->y ());
	m_update_modelview = true;
      }

  updateGL ();  
}

void
OpenglWidget::wheelEvent (QWheelEvent * e)
{ 
  e->accept ();
  if (e->delta ()<0)
    m_trackball_zoom *= 0.9;
  else
    m_trackball_zoom *= 1.1;
  
  m_update_modelview = true;
  updateGL ();
}


void
OpenglWidget::SyncContext ()
{
  if (m_update_projection)
    {
      m_update_projection = false;
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      
      float ratio = (float)m_width / (float)m_height;

      if (m_orthoview)
	gluOrtho2D (-1.0, 1.0, -1.0/ratio, 1.0/ratio);
      else
	gluPerspective (60.0, (double)m_width/(double)m_height, 0.1, 100.0);
      
      glViewport (0, 0, (GLint)m_width, (GLint)m_height);
      glMatrixMode (GL_MODELVIEW);
    } 


  if (m_update_modelview)
    {
      if (m_trackball_enable)
	{
	  Matrix4f modelview;
	  
	  m_update_modelview = false;
	  
	  m_trackball->getRotation (modelview);
	  /* Send the Matrix */
	  glLoadIdentity ();
	  gluLookAt (0, 0, 2, 0, 0, 0, 0, 1, 0);
	  glMultMatrixf ((float *)modelview[0]);
	  glScalef (m_trackball_zoom, m_trackball_zoom, m_trackball_zoom);
	  glEnable(GL_AUTO_NORMAL);
	  glEnable(GL_NORMALIZE);
	}
      else
	{
	  glLoadIdentity ();
	  //  gluLookAt (m_motion_x, m_motion_y, 2.0, 
	  //m_motion_x, m_motion_y, 0.0,   0.0, 1.0, 0.0);
	  glTranslatef (m_motion_x, m_motion_y, 0.0);
	}
    }
}

void
OpenglWidget::unProject (const Vec3f& i, Vec3f& o)
{
  double mv[16];
  double pj[16];
  int vp[16];
  double x, y, z;

  glGetDoublev (GL_MODELVIEW_MATRIX, mv);
  glGetDoublev (GL_PROJECTION_MATRIX, pj);
  glGetIntegerv (GL_VIEWPORT, vp);

  gluUnProject (i.x, vp[3] - i.y, 0,
              mv, pj, vp,
              &x, &y, &z);

  o.x = x;
  o.y = y;
  o.z = z;
}

void 
OpenglWidget::drawPolygons (const std::vector<Vec3f>& points,
			    const std::vector<std::vector<int> >& faces)
{
  std::vector<std::vector<int> >::const_iterator i;
  std::vector<int>::const_iterator j;

  float colors[3][4] = {{1.f, 0.f, 0.f, 1.f}, {0.f, 1.f, 0.f, 1.f}, 
			{0.f, 0.f, 1.f, 1.f}};
  
  glPushAttrib(~0);

  if (m_wireframe)  
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); 
  
  if (m_lighting)
    {
      glEnable(GL_LIGHTING); 
      glEnable(GL_LIGHT0);
      glEnable (GL_COLOR_MATERIAL);
    }

  int current = 0;
  for (i=faces.begin (); i!= faces.end (); i++, current++)
    {
      glBegin (GL_POLYGON);
      glColor4fv (colors[current%3]);
      for (j = (*i).begin (); j!=(*i).end (); j++)
	  glVertex3f (points[(*j)].x, points[(*j)].y, points[(*j)].z);
      glEnd ();
    }

  glPopAttrib ();
}

void 
OpenglWidget::drawPolygons (const std::vector<std::vector<Vec3f> >& faces,
			      const std::vector<std::vector<Vec3f> >& normals)
{
  std::vector<std::vector<Vec3f> >::const_iterator pi;
  std::vector<std::vector<Vec3f> >::const_iterator ni;
  assert(faces.size()==normals.size());
  std::vector<Vec3f>::const_iterator pj;
  std::vector<Vec3f>::const_iterator nj;

  GLfloat position[] = {1., 1., 10., 1.};

  glPushAttrib(~0);

  glPushMatrix ();
  glLoadIdentity();

  if (m_wireframe)  
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); 

  if (m_lighting)
    {
      glLightfv(GL_LIGHT0, GL_POSITION, position);
      glEnable(GL_LIGHTING); 
      glEnable(GL_LIGHT0);
      glShadeModel (GL_SMOOTH);
    }

  glPopMatrix ();
  int current = 0;
  for (pi=faces.begin (),
	 ni=normals.begin(); 
       pi!= faces.end (); ++pi, ++ni, current++)
    {
      glBegin (GL_POLYGON);
      assert((*pi).size() == (*ni).size());
      for (pj = (*pi).begin (),
	     nj = (*ni).begin(); pj!=(*pi).end (); ++pj, ++nj)
	{
	  glNormal3f ((*nj).x, (*nj).y, (*nj).z);
	  glVertex3f ((*pj).x, (*pj).y, (*pj).z);
	}
      glEnd ();
    }
  glPopAttrib();
}

void 
OpenglWidget::drawPolygons (const std::vector<Face>& faces)
{
  std::vector<Face>::const_iterator i;
  std::vector<Face>::const_iterator iend=faces.end();
  std::vector<Point>::const_iterator j;
  std::vector<Point>::const_iterator jend;

  glEnable (GL_COLOR_MATERIAL);
  
  for (i=faces.begin ();i!= iend; ++i)
    {
      glBegin (GL_POLYGON);
      for (j=i->begin(), jend=i->end(); j!=jend; ++j) {
	const Point p=(*j);
	Vec3f coords=p.getCoords();
	Vec3f normal=p.getNormal();
	Vec3f color=p.getColor();
	glColor3fv(&color[0]);
	glNormal3fv(&normal[0]);
	glVertex3fv(&coords[0]);
      }
      glEnd ();
    }

  if (m_normals) {
    glPushAttrib(~0);
    glDisable(GL_LIGHTING); 
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      
    glBegin (GL_LINES);
    for (i=faces.begin(); i!=iend; ++i) {
      for (j=(*i).begin(), jend=(*i).end(); j!=jend; ++j) {
	Point p=(*j);
	Vec3f coords=p.getCoords();
	Vec3f pointTo=coords+p.getNormal()/15;
	Vec3f white(1, 1, 1);
	Vec3f blue(0, 0, 1);
	glColor3fv(&white[0]);
	glVertex3fv(&coords[0]);
	glColor3fv(&blue[0]);
	glVertex3fv(&pointTo[0]);
      }
    }
    glEnd ();
    glPopAttrib();
  }
}


void
OpenglWidget::moveWindow (int relx, int rely)
{
  m_motion_x += relx / 500.0;
  m_motion_y += rely / 500.0;
}


void 
OpenglWidget::setLighting ()
{
  GLfloat position[] = {1., 1., 10., 0};
  m_lighting = !m_lighting;
  makeCurrent();
  if (m_lighting)
    {
      glPushMatrix ();
      glLoadIdentity ();
      glLightfv(GL_LIGHT0, GL_POSITION, position);
      glPopMatrix ();
      glEnable(GL_LIGHTING); 
      glEnable(GL_LIGHT0);
      glShadeModel (GL_SMOOTH);
      
      glEnable(GL_LIGHTING); 
      glEnable(GL_LIGHT0);
    } else
      glDisable(GL_LIGHTING);

  updateGL ();
}

void 
OpenglWidget::setWireframe (bool flag)
{
  m_wireframe = flag;
  makeCurrent();
  if (m_wireframe)  
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); 
  else {
    glPolygonMode(GL_BACK, GL_LINE);
    glPolygonMode(GL_FRONT, GL_FILL);
  }

  updateGL ();
}

void 
OpenglWidget::toggleNormals ()
{
  m_normals = !m_normals;
  
  updateGL ();
}
