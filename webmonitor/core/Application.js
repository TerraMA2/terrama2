var fs = require("fs");
var path = require("path");

/**
 * It defines a cache object (private) with TerraMA² Monitor settings
 * @type {Object}
 */
var _settings = {};

/**
 * It defines which context TerraMA² Monitor  should run. (default "default")
 * @type {string}
 */
var _context = "default";

/**
 * It defines a TerraMA² Monitor configuration.
 * 
 * It loads TerraMA² Monitor configuration (config/instances/*.json)
 * 
 * @class Application
 */
function Application() {
  this.load();
}

/**
 * It loads TerraMA² Monitor config json into memory.
 * 
 * @throws TypeError When read content is not a valid json
 */
Application.prototype.load = function() {

  // reading TerraMA² monitor instances configuration
  var configFiles = fs.readdirSync(path.join(__dirname, "../config/instances"));
  var configObject = {};
  configFiles.forEach(function(configFile){
    if (configFile.endsWith(".json")){
      var configFileContent = JSON.parse(fs.readFileSync(path.join(__dirname, "../config/instances/" + configFile), "utf-8"));
      var configName = configFile.split(".")[0];
      configObject[configName] = configFileContent;
    }
  });
  _settings = configObject;

};

/**
 * It sets current terrama2 context
 *
 * @throws {Error} When a contexts is not in /config/instances/*.json 
 * @param {string} context
 * @returns {void}
 */
Application.prototype.setCurrentContext = function(context) {
  if (!context) {
    return;
  }
  // checking if there is a context in configuration file
  if (_settings && !_settings.hasOwnProperty(context)) {
    var msg = "\"" + context + "\" not found in configuration file. Please check \"webapp/config/instances/*.json\"";
    throw new Error(msg);
  }

  _context = context;
};

/**
 * It retrieves a current context config
 * 
 * @returns {Object}
 */
Application.prototype.getContextConfig = function() {
  return _settings[_context];
};


/**
 * Singleton
 * @type {Application}
 */
var terrama2Application = new Application();

module.exports = terrama2Application;
