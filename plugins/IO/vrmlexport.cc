#include "plugin.h"

#define PLUGIN_NAME "VRML Export Plugin"
#define PLUGIN_MENU "ExportToVrml"
#define PLUGIN_ICON "plugins/IO/vrmlout.png"

class VRMLExport : public PluginIO 
{
public:
  
  VRMLExport() : PluginIO (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON, 
			   PLUGIN_IO_EXPORT)
  {}

};



DECLARE_PLUGIN (VRMLExport);
