#include "plugin.h"

#define PLUGIN_NAME "PolyLine Plugin"

class PolyLine : public PluginObject
{
public:
  PolyLine ();
  ~PolyLine ();
private:
  
};

PolyLine::PolyLine ()
  :PluginObject (PLUGIN_NAME)
{

}


PolyLine::~PolyLine ()
{
}


DECLARE_PLUGIN (PolyLine);
