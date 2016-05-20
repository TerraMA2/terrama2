var net = require('net');
var Promise = require('bluebird');

var Service = module.exports = function(serviceInstance) {
  this.service = serviceInstance;

  var self = this;

  self.socket = new net.Socket();

  var callbackSuccess = null;
  var callbackError = null;

  self.socket.on('data', function(byteArray) {
    console.log("client received: ", byteArray);
    console.log(byteArray.toString());

    if (callbackSuccess)
      callbackSuccess(byteArray);
  });

  self.socket.on('close', function(byteArray) {
    console.log("client closed: ", byteArray);
  });

  self.socket.on('error', function(err) {
    callbackError(err);
    console.log("client error: ", err);
  });

  self.isOpen = function() {
    return self.socket.readyState == "open";
  };

  self.connect = function() {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen()) {
        callbackError = reject;
        self.socket.connect(self.service.port, self.service.host, function() {
          resolve();
        })
      } else
        reject(new Error("Could not connect. There is a opened connection"));
    })
  };

  self.status = function(buffer) {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen())
        return reject(new Error("Could not retrieve status from closed connection"));

      callbackSuccess = resolve;
      callbackError = reject;
      self.socket.write(buffer);
    });
  };

  self.update = function(buffer) {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen())
        return reject(new Error("Could not update service from closed connection"));

      callbackError = reject;
      self.socket.write(buffer);

      resolve();
    });
  };

  self.send = function(buffer) {
    if (!this.isOpen())
      throw new Error("Could not send add data signal from closed connection");  

    self.socket.write(buffer);
  };

  self.stop = function(buffer) {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen())
        return reject(new Error("Could not close a no existent connection"));

      self.socket.write(buffer);
      resolve();
    });
  };
};