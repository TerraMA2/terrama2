"use strict";

/**
 * Socket responsible for checking the ssh connection with a given host.
 * @class SSHConnectionChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberSSHDispatcher - Ssh class.
 * @property {fs} fs - NodeJS FS module
 */
var SSHConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Ssh class
  var memberSSHDispatcher = require("../core/executors/SSH");

  var fs = require('fs');

  var Application = require('../core/Application');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // Ssh connection request event
    client.on('testSSHConnectionRequest', function(json) {
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
          client.emit('testSSHConnectionResponse', returnObject);
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

        client.emit('testSSHConnectionResponse', returnObject);
      }).catch(function(err) {
        returnObject.error = true;
        returnObject.message = err.toString();

        client.emit('testSSHConnectionResponse', returnObject);
      });
    });
  });
};

module.exports = SSHConnectionChecker;
