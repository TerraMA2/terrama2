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
    var client = new Client({
      user: self.params[self.syntax().USER],
      pass: self.params[self.syntax().PASSWORD],
      host: self.params[self.syntax().HOST],
      port: self.params[self.syntax().PORT]
    });

    client.on('error', function(err) {
      var error;
      switch (err.code) {
        case "ENOTFOUND":
        case 421:
          error = new Exceptions.ConnectionError("Host not found");
          break;
        case "ECONNREFUSED":
        case 530:
          error = new Exceptions.ConnectionError("Connection refused. Username or password does not match");
          break;
        default:
          error = new Exceptions.ConnectionError("Error in connection");
      }

      reject(error);
    });

    client.raw.quit(function(err, data) {
      if (err)
        return reject(err)

      resolve();
    });
  });
};

FtpRequest.fields = function() {
  return Utils.makeCommonRequestFields("FTP", 21, null, [UriPattern.HOST, UriPattern.PORT, UriPattern.PATHNAME],
         Utils.getCommonRequestFields().concat([{key: UriPattern.PATHNAME, type: Form.Field.TEXT, htmlClass: 'col-md-12 terrama2-schema-form'}]));
};

module.exports = FtpRequest;
