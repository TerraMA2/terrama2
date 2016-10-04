'use strict';

var AbstractRequest = require('./AbstractRequest');
var Client = require('ftp');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var Form = require("./Enums").Form;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");


var FtpRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

FtpRequest.prototype = Object.create(AbstractRequest.prototype);
FtpRequest.prototype.constructor = FtpRequest;

FtpRequest.prototype.request = function() {
  var self = this;
  return  new Promise(function(resolve, reject) {
    var config = {
      user: self.params[self.syntax().USER],
      password: self.params[self.syntax().PASSWORD],
      host: self.params[self.syntax().HOST],
      port: self.params[self.syntax().PORT]
    };

    var client = new Client();
    var path = self.params[self.syntax().PATHNAME];

    client.on('error', function(err) {
      var error;
      var syntax = self.syntax();
      switch (err.code) {
        case "ENOTFOUND":
        case 421:
          var generic = "Host not found";
          var hostError = new Exceptions.ValidationErrorItem(generic, syntax.HOST, config.user);
          error = new Exceptions.ConnectionError("Host not found", [hostError]);
          break;
        case "ECONNREFUSED":
        case 530:
          var genericMessage = "Username or password does not match";
          var userError = new Exceptions.ValidationErrorItem(genericMessage, syntax.USER, config.user);
          var passwordError = new Exceptions.ValidationErrorItem(genericMessage, syntax.PASSWORD, config.pass);
          error = new Exceptions.ConnectionError("Connection refused. " + genericMessage, [userError, passwordError]);
          break;
        default:
          error = new Exceptions.ConnectionError("Error in connection");
      }

      reject(error);
    });

    // success state
    client.on('ready', function() {
      if (path) {
        client.list(path, function(err, list) {
          client.end();
          if (err) {
            console.log(err);
            var genericMessage = "";
            switch(err.code) {
              case "EACCES":
                genericMessage = "Permission denied in \"" + path + "\"";
                break;
              case 450: // ENONT: No such file
                genericMessage = "Path \"" + path + "\" not found.";
                break;
              default:
                genericMessage = err.message;
            }

            var pathError = new Exceptions.ValidationErrorItem(genericMessage, self.syntax().PATHNAME, path);
            return reject(new Exceptions.ConnectionError("Path error. " + genericMessage, [pathError]));
          }
          resolve();
        })
      } else {
        client.end();
        resolve();
      }

    });

    // connecting to server
    client.connect(config);
  });
};

FtpRequest.fields = function() {
  return Utils.makeCommonRequestFields("FTP", 21, null, [UriPattern.HOST, UriPattern.PORT, UriPattern.PATHNAME],
         Utils.getCommonRequestFields().concat([{key: UriPattern.PATHNAME, type: Form.Field.TEXT, htmlClass: 'col-md-12 terrama2-schema-form'}]));
};

module.exports = FtpRequest;
