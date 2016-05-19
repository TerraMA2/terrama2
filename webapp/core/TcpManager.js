var net = require('net');
var Signals = require('./Signals.js');
var SSH = require("./SSHDispatcher");
var Promise = require('bluebird');
var Utils = require('./Utils');
var _ = require('lodash');
var Service = require('./Service');

var TcpManager = module.exports = {};


/**
This method parses the bytearray received.
@param {Buffer} byteArray - a nodejs buffer with bytearray received
@param {Object} object - a javascript object with signal, message and size
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


/** 
The structure looks like: 

@example 
{
  servicename..: {socket: clientSocket, service: ServiceInstance}
}
*/
var clients = {};


function _getClient(connection) {
  return new Promise(function(resolve, reject) {
    var client;
    // checking if exists
    for (var key in clients) {
      if (clients.hasOwnProperty(key)) {
        if (clients[key].service.name == connection.name) {
          client = clients[key];
          break;
        }
      }
    }

    var closeCallbackCalled = false;
    var exception;

    if (!client) {
      client = {
        socket: new net.Socket(),
        service: connection
      };

      clients[connection.name] = client;

      // connect
      client.socket.connect(connection.port, connection.host, function() {

        return resolve(client);
      });

      client.socket.on('error', function(err) {
        console.log("\n\n[ERROR] ", err);

        exception = new Error(err);
        reject(exception);
      });

      client.socket.on('close', function() {
        console.log('\n\n[CLIENT] Connection closed');
        if (!exception)
          resolve();
      });
    } else {
      if (client.socket.readyState == "open")
        return resolve(client);
      else {
        client.socket.connect(connection.port, connection.host, function () {

          return resolve(client);
        });
      }
    }
  });
}

/**
This method prepares a bytearray to send in tcp socket.
@param {Signals} signal - a valid terrama2 tcp signal
@param {Object} object - a javascript object message to send
*/
function makeBuffer(signal, object) {
  try {
    if(isNaN(signal))
     throw TypeError(signal + " is not a valid signal!");

    var totalSize;
    var jsonMessage;

    var hasMessage = !_.isEmpty(object);

    if (hasMessage) {
      jsonMessage = JSON.stringify(object).replace(/\":/g, "\": ");

      // The size of the message plus the size of two integers, 4 bytes each
      totalSize = jsonMessage.length + 4;
    } else
      totalSize = 4;
  

    // Creates the buffer and fills it with zeros
    var buffer = new Buffer(totalSize + 4);

    if (hasMessage) {
      // Writes the message (string) in the buffer with UTF-8 encoding
      buffer.write(jsonMessage, 8, jsonMessage.length);
    }

    // Writes the buffer size (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(totalSize, 0);

    // // Writes the signal (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(signal, 4);
  } catch (e) {
    throw e;
  }

  return buffer;
}

/**
This method sends a ADD_DATA_SIGNAL with bytearray to tcp socket. It is async
@param {ServiceInstance} serviceInstance - a terrama2 service instance 
@param {Object} data - a javascript object message to send
*/
TcpManager.sendData = function(serviceInstance, data) {
  try {
    var buffer = makeBuffer(Signals.ADD_DATA_SIGNAL, data);

    console.log(buffer);
    
    // getting client and writing in the channel
    _getClient(serviceInstance).then(function(client) {
      client.socket.write(buffer);
    }).catch(function(err) {
      throw err;
    });

  } catch (e) {
    console.log(e);
    throw new Error("Could not send data to service", e);
  }
};


/**
This method sends a UPDATE_SERVICE_SIGNAL with service instance values to tcp socket.
@param {ServiceInstance} serviceInstance - a terrama2 service instance 
@return {Promise} a bluebird promise
*/
TcpManager.updateService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    try {
      var buffer = makeBuffer(Signals.UPDATE_SERVICE_SIGNAL, serviceInstance.toObject());
      _getClient(serviceInstance).then(function(client) {
        // sending buffer (async)
        client.socket.write(buffer);

        resolve();
      }).catch(function(err) {
        reject(err);
      });

      resolve();
    } catch (e) {
      reject(e)
    }
  });
};

/**
This method connects via ssh to service host and sends terminal command to start service.
@param {ServiceInstance} serviceInstance - a terrama2 service instance 
@return {Promise} a bluebird promise
*/
TcpManager.startService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    // ssh structure
    var ssh = new SSH();

    ssh.connect(serviceInstance).then(function() {

      ssh.startService().then(function(code) {
        resolve(code)
      }).catch(function(err, errCode) {
        reject(err, errCode);
      });

    }).catch(function(err) {
      console.log('ssh startservice error')
      reject(err);
    });
  })
}

/**
This method sends STATUS_SIGNAL and waits for tcp client response.
@param {ServiceInstance} serviceInstance - a terrama2 service instance 
@return {Promise} a bluebird promise
*/
TcpManager.statusService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    var closeCallbackCalled = true;
    try {
      var buffer = makeBuffer(Signals.STATUS_SIGNAL, {});
      console.log(buffer);

      _getClient(serviceInstance).then(function(client) {
        // sending buffer
        client.socket.write(buffer);

        closeCallbackCalled = false;

        client.socket.on('data', function(data) {
          console.log('\n\n[CLIENT-status] Received:\n');
          console.log("BYTES: ", data);
          console.log("JSON: ", data.toString());
          try {
            var parsedMessage = parseByteArray(data);

            if (!closeCallbackCalled)
              return resolve(parsedMessage);
          } catch(e) {
            if (!closeCallbackCalled)
              return reject(e);
          }
        });

      }).catch(function(err) {
        reject(err);
      });

    } catch (e) {
      reject(e)
    }

  });
}

// async
TcpManager.stopService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    try {
      var buffer = makeBuffer(Signals.TERMINATE_SERVICE_SIGNAL, {});

      var closeCallbackCalled = true;
      _getClient(serviceInstance).then(function(client) {

        closeCallbackCalled = false;
        // sending buffer
        client.socket.write(buffer);

        client.socket.on('data', function(data) {
          console.log('\n\n[CLIENT-stop] Received:\n');
          console.log("BYTES: ", data);
          console.log("JSON: ", data.toString());
          try {
            var parsedMessage = parseByteArray(data);

            if (!closeCallbackCalled)
              return resolve(parsedMessage);
          } catch(e) {
            if (!closeCallbackCalled)
              return reject(e);
          }
        });

        client.socket.on('end', function() {
          console.log("end connection");
          resolve();
        })

        // resolve();
      }).catch(function(err) {
        reject(err);
      });

    } catch(e) {
      reject(e);
    }
  });
};

TcpManager.disconnect = function() {
  return new Promise(function(resolve, reject) {
    ssh.disconnect().then(function() {
      resolve();
    }).catch(function(err) {
      reject(err);
    });
  })
}
