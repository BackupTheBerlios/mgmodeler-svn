#include "plugin.h"

#define  NAME "VRML Input/Output Plugin"

class IOVRML : public Plugin
{
public:
  IOVRML ();
  ~IOVRML ();
};


IOVRML::IOVRML ()
  :Plugin (PLUGIN_IO, NAME)
{
}



DECLARE_PLUGIN (IOVRML);
