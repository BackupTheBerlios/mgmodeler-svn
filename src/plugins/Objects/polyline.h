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
  void buttonDown (QMouseEvent::ButtonState button, 
		   QMouseEvent::ButtonState state, const Vec3f& v);
  bool buttonUp (QMouseEvent::ButtonState button, 
		 QMouseEvent::ButtonState state, const Vec3f& v);
  void endObject ();
  bool doubleClick (QMouseEvent::ButtonState  button,
		    QMouseEvent::ButtonState state, const Vec3f& v);
  bool hasPoint (const Vec3f& v);
  Vec3f *getPoint (const Vec3f& v);
  std::list<Vec3f *>::iterator getPointIterator (const Vec3f& v);
  void mouseMove (QMouseEvent::ButtonState state, const Vec3f& v);
  void removePoint (const Vec3f& v);
  void display ();
  void drawPoints (std::list<Vec3f *>::iterator nearest);
  float distanceToSegment (const Vec3f& pt, const Vec3f& a, const Vec3f& b);
  void evaluate (std::vector<Vec3f>& res);
  void evaluateTimeline (std::vector<float>& time);
  void evaluateNormals (std::vector<Vec3f>& time);
  
 protected:
  std::list<Vec3f *> pts;
  Vec3f *selected;
  Vec3f cursor;
 private:
  bool should_close;
  
};

#endif /* POLYLINES_H */
