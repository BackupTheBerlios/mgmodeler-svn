#ifndef POLYLINES_H
#define POLYLINES_H

class PolyLine : public PluginObject
{
public:
  PolyLine ();

  /* for nurbs */
  PolyLine (const std::string& name, const std::string& menu,  
      const std::string& filename);
  ~PolyLine ();
  void buttonDown (QMouseEvent::ButtonState button, double, double, double);
  bool buttonUp (QMouseEvent::ButtonState button, double, double, double);
  void endObject ();
  bool doubleClick (QMouseEvent::ButtonState  button, double, double, double);
  bool hasPoint (double, double, double);
  void removePoint (double, double, double);
  void display ();
  void drawPoints ();
protected:
  std::list<Vec3d *> pts;
};

#endif /* POLYLINES_H */
