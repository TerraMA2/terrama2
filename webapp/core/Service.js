var net = require('net');
var Promise = require('bluebird');
var Utils = require('./Utils');
var EventEmitter = require('events').EventEmitter;
var NodeUtils = require('util');
var Signals = require('./Signals')


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

  var jsonMessage;

  if (rawData.length === 0)
    jsonMessage = {};
  else
    jsonMessage = JSON.parse(rawData);

  return {
    size: messageSizeReceived,
    signal: signal,
    message: jsonMessage
  }
}


var Service = module.exports = function(serviceInstance) {
  EventEmitter.call(this);
  this.service = serviceInstance;

  var self = this;
  self.answered = false;
  self.logs = [];

  self.socket = new net.Socket();

  var callbackSuccess = null;
  var callbackError = null;

  self.writeData = function(buffer, timeout, timeoutCallback, writeCallback) {
    self.socket.write(buffer, writeCallback || function() {});

    if (timeout) {
      timeoutCallback = timeoutCallback || function() { };
      self.socket.setTimeout(timeout, timeoutCallback);
    }
  }

  self.isRegistered = function() {
    // TODO: better implementation
    // checking a event
    return self.listenerCount('status') > 0 ||
           self.listenerCount('log') > 0 ||
           self.listenerCount('stop') > 0 ||
           self.listenerCount('close') > 0 ||
           self.listenerCount('error') > 0;
  };

  self.socket.on('data', function(byteArray) {
    self.answered = true;
    console.log("client received: ", byteArray);
    console.log("client " + self.service.name +" received: ", byteArray.toString());

    try  {
      var parsed = parseByteArray(byteArray);

      switch(parsed.signal) {
        case Signals.LOG_SIGNAL:
          self.emit("log", parsed.message);
          break;
        case Signals.STATUS_SIGNAL:
          self.emit("status", parsed.message);
          break;
        case Signals.TERMINATE_SERVICE_SIGNAL:
          self.emit("stop", parsed);
          break;
      }

      if (callbackSuccess)
        callbackSuccess(parsed);
    } catch (e) {
      console.log("Error parsing bytearray: ", e);
      self.emit('error', e);
      if (callbackError)
        callbackError(e);
    }

  });

  self.socket.on('drain', function() {
    console.log('drained');
  })

  self.socket.on('close', function(byteArray) {
    self.emit('close', byteArray);
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
        reject(new Error("Could not connect. There is a open connection"));
    })
  };

  self.status = function(buffer) {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen()) {
        self.emit('error', new Error("Could not retrieve status from closed connection"));
        return;
      }

      self.answered = false;
      self.writeData(buffer, 4000, function() {
        if (!self.answered)
          self.emit("error", new Error("Status Timeout exceeded."));
      });
    });
  };

  self.update = function(buffer) {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen()) {
        self.emit('error', new Error("Could not update service from closed connection"));
        return;
      }

      callbackError = reject;
      self.writeData(buffer);

      resolve();
    });
  };

  self.send = function(buffer) {
    if (!self.isOpen()) {
      self.emit('error', new Error("Could not send data from closed connection"));
      return;
    }

    self.writeData(buffer, null, null, function() {
      console.log("Sent all");
    });
  };

  self.stop = function(buffer) {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen()) {
        self.emit('error', new Error("Could not close a no existent connection"));
        return;
      }

      callbackSuccess = resolve;
      callbackError = reject;
      self.answered = false;
      self.writeData(buffer, 5000, function() {
        if (!self.answered)
          self.emit("error", new Error("Stop Timeout exceeded."));
      });
    });
  };

  self.log = function(buffer) {
    if (!self.isOpen()) {
      self.emit('error', (new Error("Could not apply log request from a no existent connection")));
    }

    self.answered = false;
    self.writeData(buffer, 3000, function() {
      if (!self.answered)
        self.emit("error", new Error("Log Timeout exceeded."));
    });
  }
};

NodeUtils.inherits(Service, EventEmitter);
