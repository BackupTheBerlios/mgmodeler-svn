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

  int exportData (const std::string& filename,
		  const std::vector<Face>& faces)
  {
    std::vector<Face>::const_iterator i;
    int k = 0;
    FILE *file = fopen (filename.c_str (), "w");
    if (!file)
      return -2;

    fprintf (file, "DEF noname Transform {\n  translation 0. 0. 0.\n");
    fprintf (file, "  scale 0. 0. 0.\n  children [\n");
    fprintf (file, "    geometry IndexedFaceSet { \n    solid TRUE\n");
    fprintf (file, 
	     "      coord DEF coord_noname Coordinate {\n        point [\n");
    // COORDS
    for (i=faces.begin ();i!= faces.end (); ++i)
      {
	fprintf(file, "       %f %f %f,\n", (*i)[0].getCoords ()[0], 
		(*i)[0].getCoords ()[2], -(*i)[0].getCoords ()[1]);
	fprintf(file, "       %f %f %f,\n", (*i)[1].getCoords ()[0], 
		(*i)[1].getCoords ()[2], -(*i)[1].getCoords ()[1]);
	fprintf(file, "       %f %f %f,\n", (*i)[2].getCoords ()[0], 
		(*i)[2].getCoords ()[2], -(*i)[2].getCoords ()[1]);
	k+=3;
      }
    fprintf (file, "        ]\n");
    fprintf (file, "      }\n");
    fprintf (file, "      coordIndex [\n");
    // INDEX
    for (int j =0; j<k; j+=3)
      fprintf(file, "     %d, %d, %d, -1,\n", j, j+1, j+2);
    
    fprintf (file, "      ]\n    }\n  ]\n}\n");
    fclose (file);
    return 0;
  }

};



DECLARE_PLUGIN (VRMLExport);
