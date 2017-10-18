"use strict";

/**
 * Socket responsible for checking the smtp connection with a given host.
 * @class SMTPConnectionChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 */
var SMTPConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // GeorServerConnectionChecker member
  var memberSMTPConnectionChecker = require('./../core/SMTPConnectionChecker');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // SMTP connection request event
    client.on('testSMTPConnectionRequest', function(json) {
      memberSMTPConnectionChecker(json, function(response){
        client.emit('testSMTPConnectionResponse', response);
      })
    });
  });
};

module.exports = SMTPConnectionChecker;
