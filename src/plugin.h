#ifndef PLUGIN_H
#define PLUGIN_H

#include <vector>
#include <string>
#include <stdexcept>
#include <exception>
#include <qevent.h>
#include <cassert>
#include "math/vector3.h"

class Plugin;

typedef Plugin * (*FUNC_CI) ();

class Plugin {
public:
  enum PluginType {PLUGIN_OBJECT, PLUGIN_IO};

  Plugin (const std::string& name, const std::string& menu, 
	  const std::string& filename)
    :m_name (name), m_menu(menu), m_icon (filename)
  {
    
  }
  
  virtual ~Plugin () {}
  
  virtual const PluginType getType () = 0;
  const std::string& getName () {return m_name;}
  const std::string& getMenu () {return m_menu;}
  const std::string& getIcon () {return m_icon;}

  void Destroy () {
    delete this;
  }
  
  void *m_handle;
  FUNC_CI m_createinstance;
  
protected:
  std::string m_name;
  std::string m_menu;
  std::string m_icon;
};

class PluginIO : public Plugin
{
public:
  PluginIO (const std::string& name, const std::string& menu, 
	    const std::string& filename)
    :Plugin (name, menu, filename)
  { }

  const PluginType getType () {return PLUGIN_IO;}
  virtual void parse(const std::string &filename) = 0;
  virtual const std::vector<Vec3f>& getPoints() = 0;
  virtual const std::vector<std::vector<int> >& getFaces() = 0;
};

class PluginObject : public Plugin
{
public:
  PluginObject (const std::string& name, const std::string& menu,  
		const std::string& filename)
    :Plugin (name, menu, filename), resolution (10)
  { }

  const PluginType getType () {return PLUGIN_OBJECT;}


  virtual void buttonDown (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state,float x, float y, float z) = 0;
  virtual bool buttonUp (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state,float x, float y, float z) = 0;
  virtual bool doubleClick (QMouseEvent::ButtonState button, QMouseEvent::ButtonState state,float x, float y, float z) = 0;

  virtual void mouseMove (QMouseEvent::ButtonState state, float x, float y, float z) = 0;
  virtual void endObject () = 0;

  virtual bool hasPoint (float x, float y, float z) = 0;
  virtual void removePoint (float x, float y, float z) = 0;
  virtual void display () = 0;
  virtual void evaluate (std::vector<Vec3f>& res) = 0;
  virtual void evaluateNormals (std::vector<Vec3f>& normals) = 0;
  virtual void evaluateTimeline (std::vector<float>& time) = 0;
  void setResolution (int res) {
    assert (res>0);
    resolution = res;
  }
 protected:
  int resolution;
};

class PluginManager
{
public:

  PluginManager (std::string filename);

  static void loadPlugin (std::string filename)
    throw (std::runtime_error);

  static void loadPlugins (std::string basedir)
    throw (std::runtime_error);

  static std::vector<Plugin *>::iterator begin () {return s_plugins.begin ();}
  static std::vector<Plugin *>::iterator end () {return s_plugins.end ();}

  static std::vector<Plugin *>::iterator iobegin () {return s_io_plugins.begin ();}
  static std::vector<Plugin *>::iterator ioend () {return s_io_plugins.end ();}

  static int pluginsCount () {return s_plugins.size ();}

  static Plugin *getPlugin (int i) {return s_plugins[i];}
  static Plugin *getIOPlugin (int i) {return s_io_plugins[i];}

private:
  static std::vector<Plugin *> s_plugins;
  static std::vector<Plugin *> s_io_plugins;
};


#define DECLARE_PLUGIN(CLASS)\
extern "C" Plugin *CreateInstance () {\
return new CLASS ();\
}


#endif


