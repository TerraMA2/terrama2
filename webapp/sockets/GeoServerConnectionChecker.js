"use strict";

/**
 * Socket responsible for checking the connection with a given GeoServer instance.
 * @class GeoServerConnectionChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberRequest - Request class.
 */
var GeoServerConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Request class
  var memberRequest = require('request');
  // GeorServerConnectionChecker member
  var memberGeoServerConnection = require('./../core/GeoServerConnectionChecker');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // GeoServer connection request event
    client.on('testGeoServerConnectionRequest', function(json) {
      memberGeoServerConnection(json, function(response){
        client.emit('testGeoServerConnectionResponse', response);
      })
    });
  });
};

module.exports = GeoServerConnectionChecker;
