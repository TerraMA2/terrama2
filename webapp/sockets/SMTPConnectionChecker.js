"use strict";

/**
 * Socket responsible for checking the smtp connection with a given host.
 * @class SMTPConnectionChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberSMTPConnection - SMTP class.
 */
var SMTPConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // SMTP class
  var memberSMTPConnection = require('nodemailer/lib/smtp-connection');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // SMTP connection request event
    client.on('testSMTPConnectionRequest', function(json) {
      var returnObject = {
        error: false,
        message: ""
      };

      var options = {
        host: json.host,
        port: json.port
      };

      var auth = {
        credentials: {
          user: json.username,
          pass: json.password
        }
      };

      var connection = new memberSMTPConnection(options);

      connection.on('connect', function() {
        connection.login(auth, function(err) {
          if(err) {
            returnObject.error = true;
            returnObject.message = "Username or password does not match";
          } else {
            returnObject.error = false;
            returnObject.message = "Success";
          }

          connection.quit();
          client.emit('testSMTPConnectionResponse', returnObject);
        });
      });

      connection.on('error', function(err) {
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

        client.emit('testSMTPConnectionResponse', returnObject);
      });

      connection.connect();
    });
  });
};

module.exports = SMTPConnectionChecker;
