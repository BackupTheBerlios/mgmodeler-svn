#include "plugin.h"

#include <iostream>
#include <stdio.h>

#define PLUGIN_NAME "VRML Export Plugin"
#define PLUGIN_MENU "ExportToVrml"
#define PLUGIN_ICON "plugins/IO/vrmlout.png"

class VRMLExport : public PluginIO 
{
public:
  
  VRMLExport() : PluginIO (PLUGIN_NAME, PLUGIN_MENU, PLUGIN_ICON, 
			   PLUGIN_IO_EXPORT)
  {}

  int exportData (const std::string& filename )
  {
    FILE *file = fopen (filename.c_str (), "w");
    if (!file)
      return -2;

    fprintf (file, "DEF noname Transform {\n  translation 0. 0. 0.\n");
    fprintf (file, "  scale 0. 0. 0.\n  children [\n");
    fprintf (file, "  geometry IndexedFaceSet { \n    solid TRUE\n");
    fprintf (file, "  coord DEF coord_noname Coordinate {\n    point [\n");
    // COORDS
    fprintf (file, "    ]");
    fprintf (file, "  }");
    fprintf (file, "  coordIndex [\n");
    // INDEX
    fprintf (file, "  ]\n}\n");
    fclose (file);
    return 0;
  }

};



DECLARE_PLUGIN (VRMLExport);
