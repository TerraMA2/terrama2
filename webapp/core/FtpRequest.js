'use strict';

var AbstractRequest = require('./AbstractRequest');
var Client = require('jsftp');
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
      pass: self.params[self.syntax().PASSWORD],
      host: self.params[self.syntax().HOST],
      port: self.params[self.syntax().PORT]
    };

    var client = new Client(config);

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

    client.raw.quit(function(err, data) {
      if (err) {
        return reject(err);
      }

      resolve();
    });
  });
};

FtpRequest.fields = function() {
  return Utils.makeCommonRequestFields("FTP", 21, null, [UriPattern.HOST, UriPattern.PORT, UriPattern.PATHNAME],
         Utils.getCommonRequestFields().concat([{key: UriPattern.PATHNAME, type: Form.Field.TEXT, htmlClass: 'col-md-12 terrama2-schema-form'}]));
};

module.exports = FtpRequest;
