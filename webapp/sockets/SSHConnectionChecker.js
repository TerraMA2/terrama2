"use strict";

/**
 * Socket responsible for checking the ssh connection with a given host.
 * @class SSHConnectionChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberSSHDispatcher - Ssh class.
 */
var SSHConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Ssh class
  var memberSSHDispatcher = require("../core/SSHDispatcher");

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // Ssh connection request event
    client.on('testSSHConnectionRequest', function(json) {
      var returnObject = {
        error: false,
        message: ""
      };

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
