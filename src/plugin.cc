#include "plugin.h"

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <dlfcn.h>

#include <qdir.h>
#include <qstringlist.h>

std::vector<Plugin *> PluginManager::s_plugins;
std::vector<Plugin *> PluginManager::s_io_plugins;
std::map<std::string, Plugin *> PluginManager::s_map_name_to_plugin;


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
    s_map_name_to_plugin.insert (std::pair<std::string, Plugin *>(p->getName(), p));
    break;
  case Plugin::PLUGIN_IO_IMPORT:
  case Plugin::PLUGIN_IO_EXPORT:
    s_io_plugins.push_back (p);
  }
}

void 
PluginManager::loadPlugins (const QStringList& searchpath)
  throw (std::runtime_error)
{
  for (QStringList::ConstIterator it = searchpath.begin(); 
       it != searchpath.end(); ++it) {
    std::cout << "searching in "<< *it << std::endl;
    QDir  dir(*it, "*.so");
    QStringList plugs=dir.entryList();
    for (QStringList::ConstIterator pit = plugs.begin(); pit != plugs.end(); ++pit) {
      std::cout << " loading " << QDir(*it).absPath()+"/"+*pit << std::endl;
      loadPlugin (QDir(*it).absPath()+"/"+*pit);
    }
  }
}

