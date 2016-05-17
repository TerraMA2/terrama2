"use strict";

/**
 * Socket responsible for checking the ssh connection with a given host.
 * @class SSHConnectionChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberSshClient - Ssh client.
 * @property {object} memberUtils - Utils class.
 */
var SSHConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Ssh client
  var memberSshClient = require('ssh2').Client;
  // Utils class
  var memberUtils = require("../core/Utils");

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // Ssh connection request event
    client.on('testSSHConnectionRequest', function(json) {
      var returnObject = {
        error: false,
        message: ""
      };

      try {
        json.privateKey = require('fs').readFileSync(memberUtils.getUserHome() + '/.ssh/id_rsa');
      } catch(err) {
        returnObject.error = true;
        returnObject.message = err.toString();
      }

      if(!returnObject.error) {
        var sshClient = new memberSshClient();

        sshClient.on('ready', function() {
          sshClient.end();

          returnObject.message = "Success";

          client.emit('testSSHConnectionResponse', returnObject);
        });

        sshClient.on('error', function(err) {
          sshClient.end();

          returnObject.error = true;
          returnObject.message = err.toString();

          client.emit('testSSHConnectionResponse', returnObject);
        });

        try {
          sshClient.connect(json);
        } catch(err) {
          returnObject.error = true;
          returnObject.message = err.toString();

          client.emit('testSSHConnectionResponse', returnObject);
        }
      } else {
        client.emit('testSSHConnectionResponse', returnObject);
      }
    });
  });
};

module.exports = SSHConnectionChecker;
