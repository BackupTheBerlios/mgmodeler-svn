#include "plugin.h"

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include<dlfcn.h>



std::vector<Plugin *> PluginManager::s_plugins;


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


  std::cout<<"Name: "<<p->getName()<<", Type: "<<p->getType ()<<std::endl;
  
  s_plugins.push_back (p);
}

void 
PluginManager::loadPlugins (std::string basedir)
  throw (std::runtime_error)
{
  DIR *dir = opendir (basedir.c_str ());
  struct dirent *sdir;

  std::cout<<"loadPlugins\n";
  
  if (!dir)
    throw std::runtime_error ("Error opening dir: "+basedir);
  
  
  while (sdir = readdir (dir))
    {
      struct stat features;
      std::cout<<"readdir: "<<sdir->d_name<<std::endl;

      if (!stat (sdir->d_name, &features) || 
	  (S_ISREG (features.st_mode)))
	continue;
      std::cout<<"readir (load): "<<sdir->d_name<<std::endl;
      loadPlugin (std::string("plugins/bin/")+std::string (sdir->d_name));
    }
}
