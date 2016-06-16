var net = require('net');
var Promise = require('bluebird');
var Utils = require('./Utils');


/**
 This method parses the bytearray received.
 @param {Buffer} byteArray - a nodejs buffer with bytearray received
 @return {Object} object - a javascript object with signal, message and size

 */
function parseByteArray(byteArray) {
  var messageSizeReceived = byteArray.readUInt32BE(0);
  var signalReceived = byteArray.readUInt32BE(4);
  var rawData = byteArray.slice(8, byteArray.length);

  // validate signal
  var signal = Utils.getTcpSignal(signalReceived);
  var jsonMessage = JSON.parse(rawData);

  return {
    size: messageSizeReceived,
    signal: signal,
    message: jsonMessage
  }
}


var Service = module.exports = function(serviceInstance) {
  this.service = serviceInstance;

  var self = this;

  self.socket = new net.Socket();

  var callbackSuccess = null;
  var callbackError = null;

  self.socket.on('data', function(byteArray) {
    console.log("client received: ", byteArray);

    try  {
      var parsed = parseByteArray(byteArray);

      if (callbackSuccess)
        callbackSuccess(parsed);
    } catch (e) {
      console.log("Error parsing bytearray: ", e);
      if (callbackError)
        callbackError(e);
    }

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
    if (!self.isOpen())
      throw new Error("Could not send data from closed connection");

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
