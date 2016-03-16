var AbstractRequest = require('./AbstractRequest');
var Client = require('ftp');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");

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
      user: self.params.user,
      pass: self.params.password,
      host: self.params.address,
      port: self.params.port
    });
  });
};


module.exports = FtpRequest;