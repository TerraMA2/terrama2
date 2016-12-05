"use strict";

/**
 * Socket responsible for suggesting service port number.
 * @class SSHConnectionChecker
 *
 * @author Raphael Willian da Costa
 *
 * @property {object} iosocket - Sockets object.
 * @property {object} PortScanner - PortScanner class.
 */
var ServicePortChecker = function(io) {

  // Sockets object
  var iosocket = io.sockets;
  // PortScanner class
  var PortScanner = require("../core/PortScanner");

  var Utils = require('./../core/Utils');

  // Socket connection event
  iosocket.on('connection', function(client) {

    client.on('testPortNumber', function(json) {
      var returnObject = {
        error: false,
        message: ""
      };

      var host = json.host,
          port = json.port;

      PortScanner.isPortAvailable(port, host).then(function() {
        returnObject.status = 200;
        returnObject.port = port;
        returnObject.message = "OK";
      }).catch(function(err) {
        returnObject.status = 400;
        returnObject.error = true;
        returnObject.message = err.toString();
      }).finally(function() {
        client.emit('testPortNumberResponse', returnObject);
      })

    });

    // Postgis connection request event
    client.on('suggestPortNumber', function(json) {
      var returnObject = {
        error: false,
        message: ""
      };

      var host = json.host,
          ports = json.ports;

      var ordenedPorts = ports.sort();
      var size = ordenedPorts.length;
      var startPort = 6543;
      var endPort = 65535;

      if (size > 0) {
        if (size > 1)
          endPort = ordenedPorts[size-1];
      }

      PortScanner.findPortAvailable(startPort, endPort, host, ordenedPorts).then(function(port, count) {
        returnObject.status = 200;
        returnObject.port = port;
        returnObject.message = "OK";
      }).catch(function(err) {
        returnObject.status = 400;
        returnObject.error = true;
        returnObject.error = err.toString();
      }).finally(function() {
        client.emit('suggestPortNumberResponse', returnObject);
      })
    });

    client.on('disconnect', function() {
      console.log("ServicePortChecker disconnected");
    })
  });
};

module.exports = ServicePortChecker;
