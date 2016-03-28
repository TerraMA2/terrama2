var AbstractRequest = require('./AbstractRequest');
var Client = require('ftp');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var FormField = require("./Enums").FormField;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");


var FtpRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

FtpRequest.prototype = Object.create(AbstractRequest.prototype, {
  'constructor': FtpRequest
});

FtpRequest.prototype.request = function() {
  var self = this;
  return  new Promise(function(resolve, reject) {
    var client = new Client();
    client.on('ready', function() {
      client.end();
      resolve();
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

    client.connect({
      user: self.params[self.syntax().USER],
      pass: self.params[self.syntax().PASSWORD],
      host: self.params[self.syntax().HOST],
      port: self.params[self.syntax().PORT]
    });
  });
};

FtpRequest.fields = function() {
  return Utils.makeCommonRequestFields("FTP", 21, null, [UriPattern.HOST, UriPattern.PORT, UriPattern.PATHNAME], [
      UriPattern.HOST,
      UriPattern.PORT,
      UriPattern.USER,
      {
        key: UriPattern.PASSWORD,
        type: FormField.PASSWORD
      },
      UriPattern.PATHNAME
    ]);
}
module.exports = FtpRequest;