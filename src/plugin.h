#ifndef PLUGIN_H
#define PLUGIN_H

#include <vector>
#include <string>
#include <stdexcept>
#include <exception>

class Plugin;

typedef Plugin * (*FUNC_CI) ();

class Plugin {
public:
  enum PluginType {PLUGIN_OBJECT, PLUGIN_IO};

  Plugin (const std::string& name)
    :m_name (name)
  {
  }
  
  virtual ~Plugin () {}

  virtual const PluginType getType () = 0;
  const std::string& getName () {return m_name;}

  void *m_handle;
  FUNC_CI m_createinstance;
  
protected:
  std::string m_name;
};

class PluginIO : public Plugin
{
public:
  PluginIO (const std::string &name)
    :Plugin (name)
  { }

  const PluginType getType () {return PLUGIN_IO;}
};

class PluginObject : public Plugin
{
public:
  PluginObject (const std::string &name)
    :Plugin (name)
  { }

  const PluginType getType () {return PLUGIN_OBJECT;}


  virtual void beginPlot (double x, double y, double z) = 0;
  virtual void endPlot (double x, double y, double z) = 0;

  virtual void beginObject (std::string) = 0;
  virtual void endObject () = 0;

  virtual bool hasPoint (double x, double y, double z) = 0;
  virtual void removePoint (double x, double y, double z) = 0;

  virtual void removeObject (double x, double y, double z) = 0;
};

class PluginManager
{
public:

  PluginManager (std::string filename);

  static void loadPlugin (std::string filename)
    throw (std::runtime_error);

  static void loadPlugins (std::string basedir)
    throw (std::runtime_error);

private:
  static std::vector<Plugin *> s_plugins;
};


#define DECLARE_PLUGIN(CLASS)\
extern "C" Plugin *CreateInstance () {\
return new CLASS ();\
}


#endif


