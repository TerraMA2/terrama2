"use strict";

var net = require('net');
var Promise = require('bluebird');
var async = require('async');


// const values
var MAX_PORT_NUMBER = 65535;

/**
 * Class responsible for checking available ports
 */
var PortScanner = module.exports = {};

/**
 * It checks if a given port is valid
 * @param {Number} portNumber - A port number value
 */
PortScanner.isValidPort = function(portNumber) {
  return typeof portNumber === "number" && portNumber > 0 && portNumber <= MAX_PORT_NUMBER;
};

/**
 * It checks port state. It connects via socket to address:port and returns a valid port
 * @param {Number} portNumber - A port number.
 * @param {string} address - A host address.
 * @param {function} callback - A callback function for handling success and errors.
 * The callback syntax is: function(error, validPort) {}. If there is no error, the valid port will be filled. Otherwise,
 * error will be filled out.
 */
PortScanner.checkPortState = function(portNumber, address, callback) {
  var self = this;

  if (!self.isValidPort) {
    return callback(new Error("Invalid port"));
  }
  // creating new socket to test connection
  var socket = new net.Socket();

  var host = address || "127.0.0.1";

  socket.on('connect', function() {
    // reject
    callback(new Error("The port number is busy"));
    socket.end();
  });

  socket.setTimeout(400, function() {
    socket.end();
  });

  socket.on('error', function(err) {
    if (err.code === "ECONNREFUSED") {
      callback(null, portNumber); // valid port number
    } else {
      callback(err);
    }
  });

  socket.connect(portNumber, host);
};

PortScanner.isPortAvailable = function(portNumber, address) {
  var self = this;
  return new Promise(function(resolve, reject) {
    self.checkPortState(portNumber, address, function(err) {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
};

PortScanner.findPortAvailableFrom = function(array, address) {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!(array instanceof Array)) {
      return reject(new Error("Invalid array argument. It must be instance of Array"));
    }

    var count = 0;
    var size = array.length;

    var _recursive = function(err, port) {
      ++count;
      if (err) {
        return _func();
      }
      if (count === size-1) {
        return reject(new Error("Maximum exceeded"));
      }

      resolve(port);
    };

    var _func = function() {
      self.checkPortState(array[count], address, _recursive);
    };

    _func();
  });
};

/**
 * It finds a available port from start to end.
 * @param {Number} start - A start value.
 * @param {Number} end - A end value (Default is MAX_PORT_NUMBER).
 * @param {string} address - A host
 * @param {Array<Number>} except - A set of values to be skipped
 * @return {Promise<Number>} It retrieves a promise with available port
 */
PortScanner.findPortAvailable = function(start, end, address, except) {
  var self = this;
  return new Promise(function(resolve, reject) {
    var count = 0;
    var isAvailable = false;
    except = except instanceof Array ? except : [];

    end = end || MAX_PORT_NUMBER;

    var portNumber = start;

    var condition = function() {
      return isAvailable || count === (end - start + 1);
    };

    var tryNext = function(callback) {
      if (except.indexOf(portNumber) === -1) {
        self.checkPortState(portNumber, address, function(err, port) {
          // incrementing counter
          ++count;

          // check if function returned a port (valid)
          if (portNumber === port) {
            isAvailable = true;
            callback(err);
          }
          else {
            ++portNumber;
            callback(null);
          }
        });
      } else {
        ++portNumber;
        callback(null);
      }
    };

    async.until(condition, tryNext, function(err) {
      if (isAvailable) {
        return resolve(portNumber, count);
      } else if (count === (end - start + 1)) {
        return reject(new Error("Reached the maximum ports"), count);
      }
      reject(err);
    });
  });
};
