#include "plugin.h"

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include<dlfcn.h>



std::vector<Plugin *> PluginManager::s_plugins;
std::vector<Plugin *> PluginManager::s_io_plugins;


void
PluginManager::loadPlugin (std::string filename)
  throw (std::runtime_error)
{
  void *handle;
  FUNC_CI createinstance;

  handle = dlopen (filename.c_str (), RTLD_GLOBAL|RTLD_NOW);
  if (!handle)
    throw std::runtime_error ("Error in dlopen: " + std::string (dlerror ()));
    

  createinstance = (FUNC_CI)dlsym (handle, "CreateInstance");
  if (!createinstance)
    throw std::runtime_error ("No CreateInstance in plugin:"+filename+"\n");


  Plugin *p = createinstance ();

  p->m_handle = handle;
  p->m_createinstance = createinstance;


  switch (p->getType()) {
  case Plugin::PLUGIN_OBJECT:
    s_plugins.push_back (p);
    break;
  case Plugin::PLUGIN_IO:
    s_io_plugins.push_back (p);
  }
}

void 
PluginManager::loadPlugins (std::string basedir)
  throw (std::runtime_error)
{
  DIR *dir = opendir (basedir.c_str ());
  struct dirent *sdir;

  if (!dir)
    throw std::runtime_error ("Error opening dir: "+basedir);
  
  
  while (sdir = readdir (dir))
    {
      struct stat features;

      if (!stat (sdir->d_name, &features) || 
	  (S_ISREG (features.st_mode)))
	continue;

      loadPlugin (std::string("plugins/bin/")+std::string (sdir->d_name));
    }
}
