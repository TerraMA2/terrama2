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

  if (!client || !client.isOpen()) {
    client = new Service(connection);

    clients[connection.name] = client;
  }

  return client;
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
    var client = _getClient(serviceInstance);

    client.send(buffer);

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

      console.log(buffer.toString());
      var client = _getClient(serviceInstance);

      client.update(buffer).then(function() {
        resolve();
      }).catch(function(err) {
        reject(err);
      });

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

    try {
      var buffer = makeBuffer(Signals.STATUS_SIGNAL, {});
      console.log(buffer);

      var client = _getClient(serviceInstance);

      client.status(buffer).then(function(result) {
        resolve(result);
      }).catch(function(err) {
        reject(err);
      })

    } catch (e) {
      reject(e)
    }

  });
}

TcpManager.connect = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    try {
      var client = _getClient(serviceInstance);

      client.connect().then(function() {
        resolve();
      }).catch(function(err) {
        reject(err)
      })
    } catch (e) {
      reject(e);
    }
  });
}

// async
TcpManager.stopService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    try {
      var buffer = makeBuffer(Signals.TERMINATE_SERVICE_SIGNAL, {});

      var client = _getClient(serviceInstance);

      client.stop(buffer).then(function() {
        resolve();
      }).catch(function(err) {
        console.log(err);
        reject(err)
      })

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
