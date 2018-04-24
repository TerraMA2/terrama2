'use strict';

var net = require('net');
var logger = require("./Logger");
var Promise = require('bluebird');
var Utils = require('./Utils');
var EventEmitter = require('events').EventEmitter;
var NodeUtils = require('util');
var Signals = require('./Signals');

let beginOfMessage = "(BOM)\0";
let endOfMessage = "(EOM)\0";

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


  /**
   * Creates a new Buffer based on any number of Buffer
   *
   * @private
   * @param {Buffer} arguments Any number of Buffer as arguments, may have undefined itens.
   * @return {Buffer} The new Buffer created out of the list.
   */
  function _createBufferFrom() {
    let size = 0;
    for (var i = 0; i < arguments.length; i++) {
      if(arguments[i]) {
        size += arguments[i].length;
      }
    }

    let tmp = new Buffer(size);
    let offset = 0;
    for (var i = 0; i < arguments.length; i++) {
      if(arguments[i]) {
        tmp.set(new Buffer.from(arguments[i]), offset);
        offset+=arguments[i].length;
      }
    }

    return tmp;
  }

  let tempBuffer = undefined;
  let extraData = undefined;
  self.socket.on('data', function(byteArray) {
    self.answered = true;
    var formatMessage = "Socket %s received %s";
    logger.debug(Utils.format(formatMessage, self.service.name, byteArray));

    // append and check if the complete message has arrived
    tempBuffer = _createBufferFrom(tempBuffer, byteArray);

    let completeMessage = true;
    while(tempBuffer && completeMessage) {
      try  {
        let bom = tempBuffer.toString('utf-8', 0, beginOfMessage.length);
        while(tempBuffer.length > beginOfMessage.length && bom !== beginOfMessage) {
          tempBuffer = new Buffer.from(tempBuffer.slice(1));
          bom = tempBuffer.toString('utf-8', 0, beginOfMessage.length);
        }
        
        if(bom !== beginOfMessage) {
          tempBuffer = undefined;
          throw new Error("Invalid message (BOM)");
        }

        const messageSizeReceived = tempBuffer.readUInt32BE(beginOfMessage.length);
        const headerSize = beginOfMessage.length + endOfMessage.length;
        const expectedLength = messageSizeReceived + 4;
        if(tempBuffer.length < expectedLength+headerSize) {
          // if we don't have the complete message
          // wait for the rest
          completeMessage = false;
          return;
        }

        const eom = tempBuffer.toString('ascii', expectedLength + beginOfMessage.length, expectedLength+headerSize);
        if(eom !== endOfMessage) {
          tempBuffer = undefined;
          throw new Error("Invalid message (EOM)");
        }

        // hold extra data for next message
        if(tempBuffer.length > expectedLength+headerSize) {
          extraData = new Buffer.from(tempBuffer.slice(expectedLength + headerSize));
        } else {
          extraData = undefined;
        }
        
        // free any extra byte from the message
        tempBuffer = new Buffer.from(tempBuffer.slice(beginOfMessage.length, expectedLength+beginOfMessage.length));
        
        const parsed = parseByteArray(tempBuffer);

        // we got the message, empty buffer.
        tempBuffer = extraData;

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
        // we got an error, empty buffer.
        tempBuffer = undefined;
        logger.debug(Utils.format("Error parsing bytearray received from %s. %s", self.service.name, e.toString()));
        self.emit("serviceError", e);
        if (callbackError) {
          callbackError(e);
        }
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

    // let tmp = new Buffer(buffer.length+beginOfMessage.length+endOfMessage.length);
    // tmp.set(new Buffer.from(beginOfMessage), 0);
    // tmp.set(new Buffer.from(buffer), beginOfMessage.length);
    // tmp.set(new Buffer.from(endOfMessage), buffer.length+beginOfMessage.length);

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
