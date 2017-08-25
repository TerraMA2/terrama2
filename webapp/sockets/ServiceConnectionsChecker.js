"use strict";

/**
 * Socket responsible for checking the service connections.
 * @class ServiceConnectionsChecker
 *
 * @author Francisco Vinhas [francisco.neto@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 */
var ServiceConnectionsChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Member ssh connection checker
  var memberSSHConnectionChecker = require('./../core/SSHConnectionChecker');
  // GeorServerConnectionChecker member
  var memberSMTPConnectionChecker = require('./../core/SMTPConnectionChecker');
  // Service Port number checker member
  var memberServicePortNumberChecker = require("./../core/ServicePortNumberChecker");
  // GeorServerConnectionChecker member
  var memberGeoServerConnection = require('./../core/GeoServerConnectionChecker');
  // memberDbConnectionChecker member
  var memberDbConnection = require('./../core/DbConnectionChecker');

  // Socket connection event
  memberSockets.on('connection', function(client) {
    client.on('testServiceConnectionsRequest', function(json) {
      console.log(json);
    });
  });
};

module.exports = ServiceConnectionsChecker;
