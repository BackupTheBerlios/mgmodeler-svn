#ifndef POLYLINES_H
#define POLYLINES_H

#include "math/vector3.h"

class PolyLine : public PluginObject
{
public:
  PolyLine ();

  /* for nurbs */
  PolyLine (const std::string& name, const std::string& menu,  
      const std::string& filename);
  ~PolyLine ();
  void buttonDown (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state, float, float, float);
  bool buttonUp (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state,float, float, float);
  void endObject ();
  bool doubleClick (QMouseEvent::ButtonState  button, QMouseEvent::ButtonState state,float, float, float);
  bool hasPoint (float, float, float);
  Vec3f *getPoint (float x, float y, float z);
  std::list<Vec3f *>::iterator getPointIterator (float x, float y, float z);
  void mouseMove (QMouseEvent::ButtonState state, float x, float y, float z);
  void removePoint (float, float, float);
  void display ();
  void drawPoints (std::list<Vec3f *>::iterator nearest);
  float distanceToSegment (const Vec3f& pt, const Vec3f& a, const Vec3f& b);
 protected:
  std::list<Vec3f *> pts;
  Vec3f *selected;
  Vec3f cursor;
 private:
  bool should_close;
};

#endif /* POLYLINES_H */
