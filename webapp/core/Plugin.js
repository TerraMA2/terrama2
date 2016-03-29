var events = require('events');
var util = require('util');
var fs = require('fs');
var PluginError = require("./Exceptions").PluginError;
var path = require('path');
var Promise = require('bluebird');

function Plugin() {
  this.plugins = {};
}

util.inherits(Plugin, events.EventEmitter);

Plugin.prototype.initialize = function(pluginsPath) {
  var self = this;
  return new Promise(function(resolve, reject) {
    try {
      try {
        fs.mkdirSync(path.join(__dirname, "../plugins"));
      } catch(e) {
        if (e.code !== 'EEXIST')
          console.log("Failed while creating plugins folder: " + e);
      }

      var unparsedDirectories = fs.readdirSync(pluginsPath);

      unparsedDirectories.forEach(function (unparsedDir) {
        var dirPath = path.join(pluginsPath, unparsedDir);
        var stats = fs.lstatSync(dirPath);

        if (stats.isDirectory()) {
          var files = fs.readdirSync(dirPath);

          files.forEach(function (file) {
            if (file === "package.json") {
              var data = fs.readFileSync(path.join(dirPath, file), 'utf8');

              var json = JSON.parse(data);

              if (json.name && !self.plugin.plugins[json.name]) {
                if (json.PluginClass)
                  self.plugin.load(dirPath, json);
                else
                  console.log("Err: package.json does not have PluginClass. Skipping plugin " + dirPath);
              }
              else {
                console.log("Err: package.json does not have name. Skipping plugin " + dirPath);
              }
            }
          });
        }
      });
    } catch (e) {
      console.log("Exception occurred: " + e);
    } finally {
      resolve();
    }
  });
};

Plugin.prototype.activate = function(name, plugin, path, object) {
  this.plugins[name] = {
    module: plugin,
    path: path,
    object: object
  };

  this.emit("loaded", name, plugin);
};

Plugin.prototype.load = function(pluginPath, packageJson) {
  var plugin;
  var pluginName = packageJson.name;
  var pluginClass = packageJson.PluginClass;

  try {
    plugin = require(path.join(pluginPath, pluginClass));
  } catch(e) {
    this.emit("error", new PluginError("Could not load \"" + path.join(pluginPath, pluginClass) + "\""));
  }

  if (plugin && plugin.connect) {
    var object = plugin.connect.apply(null, arguments);

    this.activate(pluginName, plugin, pluginPath, object);
  }
};

Plugin.prototype.unload = function(pluginName) {
  var plugin = this.plugins[pluginName];

  // todo: check if plugin executing
  if (plugin) {
    delete this.plugins[pluginName];
    this.emit("unloaded", pluginName);
  }
};


module.exports = Plugin;