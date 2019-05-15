var fs = require("fs");
var path = require("path");
var bcrypt = require('bcrypt');

/**
 * It defines a cache object (private) with TerraMA² settings
 * @type {Object}
 */
var _data = {
  /**
   * TerraMA² webapp metadata
   * @type {Object}
   */
  "metadata": {},
  /**
   * It defines a global terrama2 settings (instances files). It contains database config, etc.
   * @type {Object}
   */
  "settings": {},
  /**
   * It defines TerraMA² semantics loaded
   * @type {any[]}
   */
  "semantics": []
};

/**
 * It defines which context TerraMA² should run. (default "default")
 * @type {string}
 */
var _context = "default";

/**
 * It defines a TerraMA² Application metadata.
 *
 * It loads WebApp metadata (package.json), Database configuration (config/instances/*.json)
 *
 * @class Application
 */
function Application() {
  this.load();
}

/**
 * It loads TerraMA² WebApp package json into memory.
 *
 * @throws TypeError When read content is not a valid json
 */
Application.prototype.load = function() {
  // reading TerraMA² webapp metadata
  var buffer = JSON.parse(fs.readFileSync(path.join(__dirname, "../package.json"), "utf-8"));

  _data.metadata.name = buffer.name;
  _data.metadata.version = buffer.version;
  _data.metadata.fullName = buffer.name + " " + buffer.version;

  // reading TerraMA² instances configuration
  var configFiles = fs.readdirSync(path.join(__dirname, "../config/instances"));
  var configObject = {};
  configFiles.forEach(function(configFile){
    if (configFile.endsWith(".json")){
      var configFileContent = JSON.parse(fs.readFileSync(path.join(__dirname, "../config/instances/" + configFile), "utf-8"));
      var configName = configFile.split(".")[0];
      configObject[configName] = configFileContent;
    }
  });
  _data.settings = configObject;

  // reading TerraMA² .json files in semantics directory
  var semanticsFiles = fs.readdirSync(path.join(__dirname, "../../share/terrama2/semantics"));
  var semanticsArray = [];
  semanticsFiles.forEach(function(semanticFile){
    if (semanticFile.endsWith(".json")){
      var semanticFileContent = JSON.parse(fs.readFileSync(path.join(__dirname, "../../share/terrama2/semantics/" + semanticFile)));
      semanticFileContent.forEach(function(semanticContent){
        semanticsArray.push(semanticContent);
      });
    }
  });
  _data.semantics = semanticsArray;
};

/**
 * It sets current terrama2 context
 *
 * @throws {Error} When a contexts is not in instances/*.json
 * @param {string} context
 * @returns {void}
 */
Application.prototype.setCurrentContext = function(context) {
  if (!context) {
    return;
  }
  // checking if there is a context in configuration file
  if (_data.settings && !_data.settings.hasOwnProperty(context)) {
    var msg = `"${context}" not found in configuration file. Please check "webapp/config/instances"`;
    throw new Error(msg);
  }

  _context = context;

  if (_data.settings[context].webAppId === undefined || _data.settings[context].webAppId === "")
    setWebAppIdToContext();
};

/**
 * It retrieves a current context config
 *
 * @returns {Object}
 */
Application.prototype.getContextConfig = function() {
  return _data.settings[_context];
};

/**
 * It retrieves all context configs
 *
 * @returns {Object}
 */
Application.prototype.getAllConfigs = function() {
  return _data.settings;
}

/**
 * It retrieves a TerraMA² running aplication settings. It contains name, version
 *
 * @param {string} settingName - Defines which values want to return. Used it as much as possible for performance reasons
 * @returns {Object}
 */
Application.prototype.get = function(settingName) {
  if (settingName) {
    return _data[settingName];
  }
  return _data;
};

/**
 * Setting web app id in context
 */
var setWebAppIdToContext = function(){
  var configFileName = path.join(__dirname, "../config/instances/" + _context + ".json");
  var configFile = require(configFileName);
  var salt = bcrypt.genSaltSync(10);
  var date = new Date().getTime().toString();
  var hashId = _context + "|||" + bcrypt.hashSync(date + _context, salt);
  configFile.webAppId = hashId;
  fs.writeFile(configFileName, JSON.stringify(configFile, null, 2), function(err){
    if (err) return console.log(err);
  });
  _data.settings[_context].webAppId = hashId;
};

/**
 * Singleton
 * @type {Application}
 */
var terrama2Application = new Application();

module.exports = terrama2Application;
