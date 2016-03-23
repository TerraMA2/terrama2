var Plugin = require("./Plugin");

var instance = new Plugin();

instance.on('loaded', function(name, plugin) {
  console.log("Plugin: " + name + " Loaded");
});

instance.on('unloaded', function(name) {
  console.log("Plugin: " + name + " has been unloaded");
});

instance.on('error', function(err) {
  console.log("Error: " + err.message);
});


module.exports = {
  plugin: instance,
  initialize: instance.initialize
};