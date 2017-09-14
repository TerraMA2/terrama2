"use strict"

/**
 * Class responsible to check connection with data base
 * @class SMTPConnectionChecker
 * 
 * @author Francisco Vinhas [francisco.neto@funcate.org.br]
 * 
 * @property {object} json - Object with param to check connection.
 * @property {object} callback - Function to callback the result.
 */

 var SMTPConnectionChecker = function(json, callback){
  // SMTP class
  var memberNodemailer = require('nodemailer');

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

      callback(returnObject);
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

          callback(returnObject);
        });
      } else {
        returnObject.error = false;
        returnObject.message = "Success";

        callback(returnObject);
      }
    }
  });
 };
 
 module.exports = SMTPConnectionChecker;