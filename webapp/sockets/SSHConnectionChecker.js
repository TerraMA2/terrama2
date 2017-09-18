"use strict";

/**
 * Socket responsible for checking the ssh connection with a given host.
 * @class SSHConnectionChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberSSHConnectionChecker - Ssh connection checker class.
 */
var SSHConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Member ssh connection checker
  var memberSSHConnectionChecker = require('./../core/SSHConnectionChecker');

  // Socket connection event
  memberSockets.on('connection', function(client) {
    // Ssh connection request event
    client.on('testSSHConnectionRequest', function(json) {
      memberSSHConnectionChecker(json, function(reponse){
        client.emit('testSSHConnectionResponse', reponse);
      });
    });
  });
};

module.exports = SSHConnectionChecker;
