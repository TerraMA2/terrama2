var Signals = require('./Signals.js');
var SSH = require("./SSHDispatcher");
var Promise = require('bluebird');
var Utils = require('./Utils');
var _ = require('lodash');
var Service = require('./Service');
var NodeUtils = require('util');
var EventEmitter = require('events').EventEmitter;
var ServiceType = require('./Enums').ServiceType;


var TcpManager = function() {
  EventEmitter.call(this);

  var self = this;

  self.on('startService', self.startService);
  self.on('stopService', self.stopService);
  self.on('sendData', self.sendData);
  self.on('removeData', self.removeData);
  self.on('logData', self.logData);
  self.on('statusService', self.statusService);
  self.on('updateService', self.updateService);
  self.on('connect', self.connect);
}

NodeUtils.inherits(TcpManager, EventEmitter);

/**
 This method prepares a bytearray to send in tcp socket.
 @param {Signals} signal - a valid terrama2 tcp signal
 @param {Object} object - a javascript object message to send
 */
TcpManager.prototype.makebuffer = function(signal, object) {
  try {
    if(isNaN(signal))
      throw TypeError(signal + " is not a valid signal!");

    var totalSize;
    var jsonMessage = "";

    var hasMessage = !_.isEmpty(object);

    if (hasMessage) {
      jsonMessage = JSON.stringify(object).replace(/\":/g, "\": ");

      // The size of the message plus the size of two integers, 4 bytes each
      totalSize = jsonMessage.length + 4;
    } else
      totalSize = 4;

    // creating buffer to store message
    var bufferMessage = Buffer.from(jsonMessage);

    // Creates the buffer to be sent
    var buffer = new Buffer(bufferMessage.length + 8);

    if (hasMessage) {
      // Writes the message (string) in the buffer with UTF-8 encoding
      bufferMessage.copy(buffer, 8, 0, bufferMessage.length);
    }

    // checking bufferMessage length. If it is bigger than jsonMessage,
    // then there are special chars and the message size must be adjusted.
    if (bufferMessage.length > jsonMessage.length)
      totalSize = bufferMessage.length + 4;

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
 This method sends a ADD_DATA_SIGNAL with bytearray to tcp socket. It is async
 @param {ServiceInstance} serviceInstance - a terrama2 service instance
 @param {Object} data - a javascript object message to send
 */
TcpManager.prototype.sendData = function(serviceInstance, data) {
  var self = this;

  try {
    var buffer = self.makebuffer(Signals.ADD_DATA_SIGNAL, data);

    console.log(buffer);
    console.log("BufferToString: ", buffer.toString());
    console.log("BufferToString size: ", buffer.length);

    // getting client and writing in the channel
    var client = _getClient(serviceInstance);

    client.send(buffer);

  } catch (e) {
    console.log(e);
    this.emit("error", serviceInstance, new Error("Could not send data to service", e));
  }
};

/**
 This method sends a REMOVE_DATA_SIGNAL with bytearray to tcp socket. It is async
 @param {ServiceInstance} serviceInstance - a terrama2 service instance
 @param {Object} data - a javascript object message to send
 */
TcpManager.prototype.removeData = function(serviceInstance, data) {
  var self = this;
  try {
    var buffer = self.makebuffer(Signals.REMOVE_DATA_SIGNAL, data);

    console.log(buffer);

    // getting client and writing in the channel
    var client = _getClient(serviceInstance);

    client.send(buffer);

  } catch (e) {
    console.log(e);
    this.emit("error", serviceInstance, new Error("Could not send data REMOVE_DATA_SIGNAL to service", e));
  }
};


/**
 This method sends a LOG_SIGNAL with bytearray to tcp socket. It is async
 @param {ServiceInstance} serviceInstance - a terrama2 service instance
 @param {Object} data - a javascript object message to send
 */
TcpManager.prototype.logData = function(serviceInstance, data) {
  var self = this;
  try {
    var buffer = self.makebuffer(Signals.LOG_SIGNAL, data);

    // TODO: make a local buffer

    console.log("Buffer: ", buffer);
    console.log("BufferToString: ", buffer.toString());


    if (serviceInstance) {
      var client = _getClient(serviceInstance);

      client.log(buffer);
      return;
    }

    for(var key in clients) {
      if (clients.hasOwnProperty(key)) {
        var client = clients[key];
        // if (client.service.service_type_id === ServiceType.COLLECTOR)
        client.log(buffer);
        break;
      }
    }

  } catch (e) {
    console.log(e);
    this.emit("error", new Error("Could not send data LOG_SIGNAL to service", e));
  }
};

/**
 This method sends a UPDATE_SERVICE_SIGNAL with service instance values to tcp socket.
 @param {ServiceInstance} serviceInstance - a terrama2 service instance
 @return {Promise} a bluebird promise
 */
TcpManager.prototype.updateService = function(serviceInstance) {
  var self = this;
  try {
    var buffer = self.makebuffer(Signals.UPDATE_SERVICE_SIGNAL, serviceInstance.toObject());

    console.log(buffer.toString());
    var client = _getClient(serviceInstance);

    client.update(buffer)

  } catch (e) {
    this.emit("error", serviceInstance, e);
  }
};

/**
 This method connects via ssh to service host and sends terminal command to start service.
 @param {ServiceInstance} serviceInstance - a terrama2 service instance
 @return {Promise} a bluebird promise
 */
TcpManager.prototype.startService = function(serviceInstance) {
  var self = this;

  // ssh structure
  var ssh = new SSH();

  ssh.connect(serviceInstance).then(function() {

    ssh.startService().then(function(code) {
      self.emit("serviceStarted", serviceInstance);
    }).catch(function(err, errCode) {
      self.emit('error', serviceInstance, err);
    });

  }).catch(function(err) {
    console.log('ssh startservice error')
    this.emit("error", serviceInstance, rr);
  });
}

/**
 This method sends STATUS_SIGNAL and waits for tcp client response.
 @param {ServiceInstance} serviceInstance - a terrama2 service instance
 @return {Promise} a bluebird promise
 */
TcpManager.prototype.statusService = function(serviceInstance) {
  var self = this;
  try {
    var buffer = self.makebuffer(Signals.STATUS_SIGNAL, {});
    console.log(buffer);

    var client = _getClient(serviceInstance);

    client.once('error', function(err) {
      self.emit('error', serviceInstance, err);
    })

    client.status(buffer);

  } catch (e) {
    this.emit("error", serviceInstance, e)
  }
}

TcpManager.prototype.connect = function(serviceInstance) {
  var self = this;
  try {
    var client = _getClient(serviceInstance);

    client.connect().then(function() {
      // preparing socket listeners
      self.initialize(client);

    }).catch(function(err) {
      console.log(err)
    }).finally(function() {
      self.emit('serviceConnected', serviceInstance);
    })
  } catch (e) {
    this.emit("error", serviceInstance, e);
  }
}

TcpManager.prototype.getService = function(serviceInstance) {
  return _getClient(serviceInstance);
};

// async
TcpManager.prototype.stopService = function(serviceInstance) {
  var self = this;
  try {
    var buffer = self.makebuffer(Signals.TERMINATE_SERVICE_SIGNAL, {});

    var client = _getClient(serviceInstance);

    client.stop(buffer);

  } catch(e) {
    this.emit("error", e);
  }

};

TcpManager.prototype.initialize = function(client) {
  var self = this;
  if (!client.isOpen())
    return;

  client.on('status', function(response) {
    self.emit('statusReceived', client.service, response)
  });

  client.on('log', function(response) {
    // TODO: make a local buffer
    self.emit('logReceived', client.service, response)
  });

  // terminate signal
  client.on('stop', function(response) {
    self.emit('stop', client.service, response);
  });

  // socket closed
  client.on('close', function(response) {
    self.emit('close', client.service, response);
  });

  client.on('error', function(response) {
    self.emit('error', client.service, response)
  })
}

TcpManager.prototype.disconnect = function() {
  return new Promise(function(resolve, reject) {
    ssh.disconnect().then(function() {
      resolve();
    }).catch(function(err) {
      reject(err);
    });
  })
}

module.exports = new TcpManager();
