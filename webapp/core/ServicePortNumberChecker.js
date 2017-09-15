"use strict"

/**
 * Class responsible to check port number
 * @class ServicePortNumberChecker
 * 
 * @author Francisco Vinhas [francisco.neto@funcate.org.br]
 * 
 * @property {object} json - Object with param to check connection.
 * @property {object} callback - Function to callback the result.
 * @property {object} PortScanner - 'Port Scanner' module.
 */

 var ServicePortNumberChecker = function(json, callback){

  // PortScanner class
  var PortScanner = require("./PortScanner");

  var returnObject = {
    checkConnection: (json.checkConnection ? true : false),
    error: false,
    message: ""
  };

  if(json.service)
    returnObject.service = json.service;

  var host = json.host,
      port = json.port;

  PortScanner.isPortAvailable(port, host).then(function() {
    returnObject.status = 200;
    returnObject.port = port;
    returnObject.message = "OK";
  }).catch(function(err) {
    returnObject.status = 400;
    returnObject.error = true;
    returnObject.port = port;
    returnObject.message = err.toString() + ", port ";
  }).finally(function() {
    callback(returnObject);
  });

 };

 module.exports = ServicePortNumberChecker;
 