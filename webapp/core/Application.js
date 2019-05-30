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
  "semantics": [],
  /**
   * Defines TerraMA2 version
   * @type {any}
   */
  "version": null
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
  // reading Version
  const version = require(path.resolve(__dirname, '../../share/terrama2/version.json'));
  _data.version = version;

  // reading TerraMA² webapp metadata
  const buffer = JSON.parse(fs.readFileSync(path.resolve(__dirname, "../package.json"), "utf-8"));

  _data.metadata.name = buffer.name;
  _data.metadata.version = buffer.version;
  _data.metadata.fullName = buffer.name + " " + buffer.version;

  // reading TerraMA² instances configuration
  const settings = require(path.resolve(__dirname, "../config/settings.json"));

  _data.settings = settings;

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

  if (_data.settings.webAppId === undefined || _data.settings.webAppId === "")
    setWebAppIdToContext();

  const dbSettings = require(path.resolve(__dirname, '../config/db.json'));

  const mode = process.NODE_ENV || 'development';

  _data.settings.db = dbSettings[mode];
};

/**
 * It retrieves a current context config
 *
 * @returns {Object}
 */
Application.prototype.getContextConfig = function() {
  return _data.settings;
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
 * Retrieves TerraMA2 version string
 * @returns {string|any}
 */
Application.prototype.getVersion = function(serialize = true) {
  if (serialize) {
    const { version } = _data;
    return `${version.major}.${version.minor}.${version.patch}-${version.tag}`;
  }

  return _data.version;
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
  var configFileName = path.join(__dirname, "../config/settings.json");
  var configFile = require(configFileName);
  var salt = bcrypt.genSaltSync(10);
  var date = new Date().getTime().toString();
  var hashId = _context + "|||" + bcrypt.hashSync(date + _context, salt);
  configFile.webAppId = hashId;
  fs.writeFileSync(configFileName, JSON.stringify(configFile, null, 2));
  _data.settings.webAppId = hashId;
};

/**
 * Singleton
 * @type {Application}
 */
var terrama2Application = new Application();

module.exports = terrama2Application;
