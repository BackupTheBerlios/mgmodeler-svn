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
  void display () const;
  void drawPoints (const std::list<Vec3f *>::const_iterator& nearest) const;
  float distanceToSegment (const Vec3f& pt, const Vec3f& a, const Vec3f& b) const;
  float evaluate (std::vector<Vec3f>& res) const;
  void evaluateTimeline (std::vector<float>& time) const;
  void evaluateNormals (std::vector<Vec3f>& time) const;
  void drawNormals () const;
 protected:
  std::list<Vec3f *> pts;
  Vec3f *selected;
  Vec3f cursor;
  mutable bool need_recompute;
 private:
  bool should_close;
};

#endif /* POLYLINES_H */
