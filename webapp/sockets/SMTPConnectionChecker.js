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
  var memberNodemailer = require('nodemailer');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // SMTP connection request event
    client.on('testSMTPConnectionRequest', function(json) {
      var returnObject = {
        error: false,
        message: "",
        sendTestEmail: (json.emailAddress ? true : false)
      };

      var options = {
        host: json.host,
        port: json.port,
        auth: {
          user: json.username,
          pass: json.password
        }
      };

      var transporter = memberNodemailer.createTransport(options);

      // Verify connection configuration
      transporter.verify(function(error, success) {
        if(error) {
          returnObject.error = true;

          if(error.code === "EAUTH") {
            returnObject.message = "Username or password does not match. Important! Verify if your email server is not blocking the connection";
          } else {
            switch(error.errno) {
              case "ENOTFOUND":
                returnObject.message = "Address not found";
                break;
              case "ETIMEDOUT":
                returnObject.message = "Connection timeout, verify the port";
                break;
              default:
                returnObject.message = "Failed to connect, verify the connection parameters";
            }
          }

          client.emit('testSMTPConnectionResponse', returnObject);
        } else {
          if(json.emailAddress) {
            var envelope = {
              from: json.username,
              to: json.emailAddress,
              subject: json.message,
              text: json.message
            };

            transporter.sendMail(envelope, function(err, info) {
              if(err) {
                returnObject.error = true;
                returnObject.message = err.message;
              } else {
                returnObject.error = false;
                returnObject.message = "Success";
              }

              client.emit('testSMTPConnectionResponse', returnObject);
            });
          } else {
            returnObject.error = false;
            returnObject.message = "Success";

            client.emit('testSMTPConnectionResponse', returnObject);
          }
        }
      });
    });
  });
};

module.exports = SMTPConnectionChecker;
