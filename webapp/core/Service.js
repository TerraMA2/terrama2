'use strict';

var net = require('net');
var logger = require("./Logger");
var Promise = require('bluebird');
var Utils = require('./Utils');
var EventEmitter = require('events').EventEmitter;
var NodeUtils = require('util');
var Signals = require('./Signals');


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

  if (rawData.length === 0) {
    jsonMessage = {};
  } else {
    jsonMessage = JSON.parse(rawData);
  }

  return {
    size: messageSizeReceived,
    signal: signal,
    message: jsonMessage
  };
}

/**
 * It handles a tcp service connection via TCP
 * 
 * @class Service
 * @param {Service} serviceInstance - A TerraMA² service instance
 * 
 * @fires Service#processFinished
 */
var Service = module.exports = function(serviceInstance) {
  EventEmitter.call(this);
  /**
   * It defines a TerraMA² service instance
   * @type {Service}
   */
  this.service = serviceInstance;

  var self = this;
  self.answered = false;
  self.logs = [];

  self.socket = new net.Socket();

  var callbackSuccess = null;
  var callbackError = null;

  self.writeData = function(buffer, timeout, timeoutCallback, writeCallback) {
    self.socket.write(buffer, writeCallback || function() {});

    if(timeout) {
      timeoutCallback = timeoutCallback || function() {};
      setTimeout(timeoutCallback, timeout);
    }
  };

  /**
   * It checks if there is already a listener registered
   * 
   * @returns {boolean}
   */
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
    var formatMessage = "Socket %s received %s";
    logger.debug(Utils.format(formatMessage, self.service.name, byteArray));
    logger.debug(Utils.format(formatMessage, self.service.name, byteArray.toString()));

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
        case Signals.PROCESS_FINISHED_SIGNAL:
          /**
           * Used to notify when a process has been finished. C++ service emits a processed data to save
           * and delivery to user
           * 
           * @event Service#processFinished
           * @type {Object}
           */
          self.emit("processFinished", parsed.message);
          break;
        case Signals.VALIDATE_PROCESS_SIGNAL:
          self.emit("validateProcess", parsed.message);
          break;
      }

      if (callbackSuccess) {
        callbackSuccess(parsed);
      }
    } catch (e) {
      logger.debug(Utils.format("Error parsing bytearray received from %s. %s", self.service.name, e.toString()));
      self.emit("serviceError", e);
      if (callbackError) {
        callbackError(e);
      }
    }

  });

  self.socket.on('drain', function() {
    logger.debug('drained');
  });

  self.socket.on('close', function(byteArray) {
    self.emit('close', byteArray);
    logger.debug(Utils.format("Socket %s closed: %s", self.service.name, byteArray));
  });

  self.socket.on('error', function(err) {
    callbackError(err);
    self.emit("serviceError", err);
    var errMessage;
    switch(err.code) {
      case "ECONNREFUSED":
        errMessage = "Connection refused.";
        break;
      default:
        errMessage = err.toString();
    }
    logger.debug(Utils.format("Socket %s error: %s", self.service.name, new Error(errMessage).toString()));
  });

  self.isOpen = function() {
    return self.socket.readyState == "open";
  };

  self.connect = function() {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen()) {
        callbackError = reject;
        self.socket.connect(self.service.port, self.service.host, function() {
          return resolve();
        });
      } else {
        return reject(new Error("Could not connect. There is a open connection"));
      }
    });
  };

  self.status = function(buffer) {
    if (!self.isOpen()) {
      self.emit("serviceError", new Error("Could not retrieve status from closed connection"));
      return;
    }

    self.answered = false;
    self.writeData(buffer, 10000, function() {
      if (!self.answered) {
        self.emit("serviceError", new Error("Status Timeout exceeded."));
      }
    });
  };

  self.update = function(buffer) {
    return new Promise(function(resolve, reject) {
      callbackError = reject;
      self.send(buffer);

      return resolve();
    });
  };

  self.send = function(buffer) {
    if (!self.isOpen()) {
      self.emit("serviceError", new Error("Could not send data from closed connection"));
      return;
    }

    self.writeData(buffer, null, null, function() {
      logger.debug("Sent all");
    });
  };

  self.stop = function(buffer) {
    return new Promise(function(resolve, reject) {
      if (!self.isOpen()) {
        self.emit("serviceError", new Error("Could not close a no existent connection"));
        return;
      }

      callbackSuccess = resolve;
      callbackError = reject;
      self.answered = false;
      self.writeData(buffer, 10000, function() {
        if (!self.answered) {
          self.emit("serviceError", new Error("Stop Timeout exceeded."));
        }
      });
    });
  };

  self.log = function(buffer) {
    if (!self.isOpen()) {
      self.emit("serviceError", (new Error("Could not apply log request from a no existent connection")));
    }

    self.answered = false;
    self.writeData(buffer, 10000, function() {
      if (!self.answered) {
        self.emit("serviceError", new Error("Log Timeout exceeded."));
      }
    });
  };
};

NodeUtils.inherits(Service, EventEmitter);
