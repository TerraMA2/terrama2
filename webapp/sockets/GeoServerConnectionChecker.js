"use strict";

/**
 * Socket responsible for checking the smtp connection with a given host.
 * @class GeoServerConnectionChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberSMTPConnection - SMTP class.
 */
var GeoServerConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // SMTP class
  //var memberRequest = require('request');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // SMTP connection request event
    client.on('testGeoServerConnectionRequest', function(json) {
      var returnObject = {
        error: false,
        message: ""
      };

      var request = require('request');

      var protocol = "http://";

      if(json.host.indexOf("http://") !== -1) {
        var host = json.host.replace("http://", "");
      } else if(json.host.indexOf("https://") !== -1) {
        var host = json.host.replace("https://", "");
        var protocol = "https://";
      } else {
        var host = json.host;
      }

      var hostArray = host.split("/");
      host = hostArray.shift();
      var path = hostArray.join("/");

      if(path.charAt(path.length - 1) == "/")
        path = path.slice(0, -1);

      var address = protocol + host + ":" + json.port + (path != "" ? "/" + path : "") + "/rest/layers";

      var options = {
        url: address,
        method: 'GET',
        auth: {
          user: json.username,
          pass: json.password
        },
        headers: {
          'accept': 'application/json',
          'content-Type': 'application/json'
        }
      };

      request(options, function(err, res, html) {
        if(err) {
          returnObject.error = true;

          switch(err.errno) {
            case "ENOTFOUND":
              returnObject.message = "Address not found";
              break;
            case "ETIMEDOUT":
              returnObject.message = "Connection timeout, verify the port";
              break;
            default:
              returnObject.message = "Failed to connect, verify the connection parameters";
          }
        } else {
          if(html.toLowerCase().indexOf("http status 401") !== -1) {
            returnObject.error = true;
            returnObject.message = "Username or password does not match";
          } else if(html.toLowerCase().indexOf("404 not found") !== -1) {
            returnObject.error = true;
            returnObject.message = "Address not found";
          } else {
            returnObject.error = false;
            returnObject.message = "Success";
          }
        }

        client.emit('testGeoServerConnectionResponse', returnObject);
      });
    });
  });
};

module.exports = GeoServerConnectionChecker;
