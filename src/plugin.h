#ifndef PLUGIN_H
#define PLUGIN_H

#include "math/vector3.h"
#include "math/point.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <exception>
#include <qevent.h>
#include <cassert>
#include <iostream>
#include <map>

#include <qstringlist.h>

class Plugin;

typedef Plugin * (*FUNC_CI) ();

class Plugin {
public:
  enum PluginType {PLUGIN_OBJECT, PLUGIN_IO_IMPORT, PLUGIN_IO_EXPORT};

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
	    const std::string& filename, 
	    const PluginType type=PLUGIN_IO_IMPORT)
    :Plugin (name, menu, filename)
  {io_type = type;}

  const PluginType getType () {return io_type;}

  virtual int importData(const std::string &filename) {return -1;}
  virtual int exportData(const std::string &filename, const std::vector<Face>&)
    {return -1;}

  virtual const std::vector<Vec3f>& getPoints() { 
    return emptyPoints;
  }
  virtual const std::vector<std::vector<int> >& getFaces() {
    return emptyFace;
  }
 private:
  std::vector<Vec3f> emptyPoints;
  std::vector<std::vector<int> > emptyFace;
  PluginType io_type;
};

class PluginObject : public Plugin
{
public:
  PluginObject (const std::string& name, const std::string& menu,  
		const std::string& filename)
    :Plugin (name, menu, filename), resolution (10)
  { }

  const PluginType getType () {return PLUGIN_OBJECT;}


  virtual void buttonDown (QMouseEvent::ButtonState button, 
			   QMouseEvent::ButtonState state, const Vec3f& v) = 0;
  virtual bool buttonUp (QMouseEvent::ButtonState button, 
			 QMouseEvent::ButtonState state, const Vec3f& v) = 0;
  virtual bool doubleClick (QMouseEvent::ButtonState button,
			    QMouseEvent::ButtonState state, const Vec3f& v) = 0;
  virtual void mouseMove (QMouseEvent::ButtonState state, const Vec3f& v) = 0;
  virtual void endObject () = 0;

  virtual bool hasPoint (const Vec3f& v) = 0;
  virtual void removePoint (const Vec3f& v) = 0;
  virtual void display () const = 0;
  virtual float evaluate (std::vector<Vec3f>& res) const = 0;
  virtual void evaluateNormals (std::vector<Vec3f>& normals) const  = 0;
  virtual void evaluateTimeline (std::vector<float>& time) const = 0;
  virtual void drawNormals () const = 0;
  virtual void setResolution (int res) {
    assert (res>0);
    resolution = res;
  }

  virtual void load (std::istream &stream) = 0;
  virtual void save (std::ostream &stream) const = 0;
  
    
 protected:
  int resolution;
};

class PluginManager
{
public:

  PluginManager (std::string filename);

  static void loadPlugin (std::string filename)
    throw (std::runtime_error);

  static void loadPlugins (const QStringList& l)
    throw (std::runtime_error);

  static std::vector<Plugin *>::iterator begin () {return s_plugins.begin ();}
  static std::vector<Plugin *>::iterator end () {return s_plugins.end ();}

  static std::vector<Plugin *>::iterator iobegin () {return s_io_plugins.begin ();}
  static std::vector<Plugin *>::iterator ioend () {return s_io_plugins.end ();}

  static int pluginsCount () {return s_plugins.size ();}

  static Plugin *getPlugin (int i) {return s_plugins[i];}
  static Plugin *getIOPlugin (int i) {return s_io_plugins[i];}
  static Plugin *getPluginByName (const std::string& name) {
    return s_map_name_to_plugin[name];
  }
private:
  static std::vector<Plugin *> s_plugins;
  static std::vector<Plugin *> s_io_plugins;
 public:
  static std::map<std::string, Plugin *> s_map_name_to_plugin;
};


#define DECLARE_PLUGIN(CLASS)\
extern "C" Plugin *CreateInstance () {\
return dynamic_cast<Plugin *> (new CLASS ());\
}


#endif


