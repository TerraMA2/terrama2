"use strict"

/**
 * Class responsible to check ssh connection
 * @class SSHConnectionChecker
 * 
 * @author Francisco Vinhas [francisco.neto@funcate.org.br]
 * 
 * @property {object} json - Object with param to check connection.
 * @property {object} callback - Function to callback the result.
 */

 var SSHConnectionChecker = function(json, callback){
  // Ssh class
  var memberSSHDispatcher = require("./executors/SSH");
  // File system class
  var fs = require('fs');
  // Application class
  var Application = require('./Application');

  var returnObject = {
    error: false,
    message: ""
  };

  var isLocal = json.isLocal;
  var pathToBinary = json.pathToBinary || "";

  // TODO: local validation (pathToBinary)
  if (isLocal) {

    try {
      // Query the entry
      var stats = fs.lstatSync(pathToBinary);

      // Is it a file?
      if (stats.isFile()) {
        returnObject.message = "Success";
      } else {
        if (!stats.isDirectory() && (stats.mode & 1)) { //TODO: improve it. When is executable, the current solution is checking mode "x"
          returnObject.message = "Success";
        } else {
          throw new Error("Invalid service executable");
        }
      }
    } catch (e) {
      var config = Application.getContextConfig();
      var tryDefaultPath = (pathToBinary === config.defaultExecutableName);
      var enoentError = "No such file \""+ pathToBinary +"\" in directory";

      if(tryDefaultPath) {
        pathToBinary = (config.defaultExecutablePath.endsWith("/") ? config.defaultExecutablePath : config.defaultExecutablePath + "/") + config.defaultExecutableName;

        try {
          // Query the entry
          var stats = fs.lstatSync(pathToBinary);

          // Is it a file?
          if (stats.isFile()) {
            returnObject.message = "Success";
          } else {
            if (!stats.isDirectory() && (stats.mode & 1)) { //TODO: improve it. When is executable, the current solution is checking mode "x"
              returnObject.message = "Success";
            } else {
              throw new Error("Invalid service executable");
            }
          }
        } catch (error) {
          returnObject.error = true;
          var message = "";
          switch (e.code) {
            case "ENOENT":
              message = enoentError;
              break;
            default:
              message = e.toString();
          }
          returnObject.message = message;
        }
      } else {
        returnObject.error = true;
        var message = "";
        switch (e.code) {
          case "ENOENT":
            message = "No such file \""+ pathToBinary +"\" in directory";
            break;
          default:
            message = e.toString();
        }
        returnObject.message = message;
      }
    } finally {
      callback(returnObject);
      return;
    }
  }

  var serviceInstance = {
    host: json.host,
    sshPort: json.port,
    sshUser: json.username
  };

  var ssh = new memberSSHDispatcher();

  ssh.connect(serviceInstance).then(function() {
    ssh.disconnect();

    returnObject.message = "Success";

    callback(returnObject);
  }).catch(function(err) {
    returnObject.error = true;
    returnObject.message = err.toString();

    callback(returnObject);
  });

 };

 module.exports = SSHConnectionChecker;