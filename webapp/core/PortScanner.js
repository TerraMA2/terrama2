var net = require('net');
var Promise = require('bluebird');
var async = require('async')


// const values
var MAX_PORT_NUMBER = 65535;

var PortScanner = module.exports = {};

PortScanner.isValidPort = function(portNumber) {
  return typeof portNumber === "number" && portNumber > 0 && portNumber <= MAX_PORT_NUMBER;
}

PortScanner.checkPortState = function(portNumber, address, callback) {
  var self = this;

  if (!self.isValidPort)
    return callback(new Error("Invalid port"));

  // creating new socket to test connection
  var socket = new net.Socket();

  var host = address || "127.0.0.1";

  socket.on('connect', function() {
    // reject
    callback(new Error("Port number busy"));
    socket.end();
  });

  socket.setTimeout(400, function() {
    socket.end();
  })

  socket.on('error', function(err) {
    if (err.code === "ECONNREFUSED")
      callback(null, portNumber); // valid port number
    else
      callback(err);
  })

  socket.connect(portNumber, host);
}

PortScanner.isPortAvailable = function(portNumber, address) {
  var self = this;
  return new Promise(function(resolve, reject) {
    self.checkPortState(portNumber, address, function(err) {
      if (err)
        reject(err);
      else
        resolve();
    });
  });
}

PortScanner.findPortAvailableFrom = function(array, address) {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!array instanceof Array)
      return reject(new Error("Invalid array argument. It must be instance of Array"));

    var count = 0;
    var size = array.length;

    var _recursive = function(err, port) {
      ++count;
      if (err) {
        return _func()
      }

      if (count === size-1)
        return reject(new Error("Maximum exceeded"));

      resolve(port);
    }

    var _func = function() {
      self.checkPortState(array[count], address, _recursive);
    }

    _func()
  });
}

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
    }

    async.until(condition, tryNext, function(err) {
      if (isAvailable)
        return resolve(portNumber, count);
      else if (count === (end - start + 1))
        return reject(new Error("Reached the maximum ports"), count);
      reject(err);
    })
  });
}
