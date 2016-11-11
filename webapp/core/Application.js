var fs = require("fs");
var path = require("path");

var _data = {};

function Application() {
  this.load();
}

/**
 * It loads TerraMA² WebApp package json into memory.
 * 
 * @throws TypeError When read content is not a valid json
 */
Application.prototype.load = function() {
  var buffer = JSON.parse(fs.readFileSync(path.join(__dirname, "../package.json"), "utf-8"));

  _data.name = buffer.name;
  _data.version = buffer.version;
  _data.fullName = buffer.name + " " + buffer.version;
};

/**
 * It retrieves a copy of TerraMA² running aplication metadata. It contains name, version
 * @returns {Object}
 */
Application.prototype.get = function() {
  return Object.assign({}, _data);
};

/**
 * Singleton
 * @type {Application}
 */
var terrama2Application = new Application();

module.exports = terrama2Application;
