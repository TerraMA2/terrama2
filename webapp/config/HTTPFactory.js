// NodeJS dependencies
var fs = require("fs");

/**
 * It defines a HTTP factory for handling HTTP and HTTPs protocols.
 */
var HttpFactory = module.exports = {};

/**
 * It configures a SSL options if there is in settings.
 * 
 * @example
 * 
 * {default: {..., ssl: true}} - It will use default SSL path (webapp/config/key|cert)
 * 
 * {default: {..., ssl: {key: pathToKey, cert: pathToCert}}} - It will use custom key path (Remember it based in webapp/config path if not starts with slash) 
 */
function setup (ssl) {
  if (ssl) {
    return {
      key  : fs.readFileSync(ssl.key || "key.pem"),
      cert : fs.readFileSync(ssl.certificate || "cert.pem")
    };
  }
}

/**
 * It builds a HTTP or HTTPS server and start
 * 
 * @param {Express} app - NodeJS express
 * @param {any?} options - Server options
 * @returns {Server}
 */
function startServer(app, options) {
  if (options) {
    return require('https').createServer(options, app);
  }
  return require('http').createServer(app);
}

/**
 * It creates a HTTP or HTTPS server based in TerraMA² Settings (Look in Application.js)
 * @param {Express} app - NodeJS express
 * @param {Object} settings - TerraMA² Settings
 * @returns {Server}
 */
HttpFactory.create = function(app, settings) {
  var options = setup(settings.ssl);
  return startServer(app, options);
};