var Signals = require('./Signals.js');
var SSH = require("./SSHDispatcher");
var Utils = require('./Utils');
var _ = require('lodash');
var Service = require('./Service');
var NodeUtils = require('util');
var EventEmitter = require('events').EventEmitter;
var ServiceType = require('./Enums').ServiceType;
var Process = require('./Process');
var Executor = require('./Executor');
var Promise = require('bluebird');


var TcpManager = function() {
  EventEmitter.call(this);

  this.on('startService', this.startService);
  this.on('stopService', this.stopService);
  this.on('sendData', this.sendData);
  this.on('removeData', this.removeData);
  this.on('logData', this.logData);
  this.on('statusService', this.statusService);
  this.on('updateService', this.updateService);
  this.on('connect', this.connect);

  this.registered = false;
};

NodeUtils.inherits(TcpManager, EventEmitter);


/**
 This method prepares a bytearray to send in tcp socket.
 */
TcpManager.prototype.isRegistered = function() {
  return this.registered;
};


/**
 This method prepares a byte array to send in tcp socket.
 @param {Signals} signal - a valid TerraMA2 tcp signal
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
    var bufferMessage = new Buffer(jsonMessage);

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
};

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
  It will store c++ log application.
*/
var logs = {
  collectors: [],
  analysis: []
};

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

    // checking if there are active connections
    if (Object.keys(clients).length === 0)
      throw new Error("There is no client to log request. Start services in Admin Dashboard");

    var buffer = self.makebuffer(Signals.LOG_SIGNAL, data);

    console.log("Buffer: ", buffer);
    console.log("BufferToString: ", buffer.toString());

    var client = _getClient(serviceInstance);

    // checking first attempt when there is no active socket (listing services)
    if (!client.isOpen()) {
      self.emit('error', client.service, new Error("There is no active connection"));
      return;
    }

    var begin = data.begin;
    var end = data.end;

    var array = [];
    switch (client.service.service_type_id) {
      case ServiceType.COLLECTOR:
        array = logs.collectors;
        break;
      case ServiceType.ANALYSIS:
        array = logs.analysis;
        break;
      default:
        this.emit("error", null, new Error("Invalid service type id"));
        return;
    }

    // checking server cache
    if (end <= array.length) {
      self.emit('logReceived', client.service, array.slice(begin, end));
      return;
    }

    // requesting for log
    client.log(buffer);

  } catch (e) {
    console.log(e);
    this.emit("error", serviceInstance, new Error("Could not send data LOG_SIGNAL to service", e));
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

  return new Promise(function(resolve, reject) {
    var instance = new Process();
    if (serviceInstance.host && serviceInstance.host !== "") {
      instance.setAdapter(new SSH());
    } else {
      instance.setAdapter(new Executor());
    }

    instance.connect(serviceInstance).then(function() {
      instance.startService().then(function(code) {
        // self.emit("serviceStarted", serviceInstance);
        resolve(code);
      }).catch(function(err) {
        // self.emit('error', serviceInstance, err);
        reject(err);
      }).finally(function() {
        instance.disconnect();
      })
    }).catch(function(err) {
      console.log('ssh startservice error')
      console.log(err);
      reject(err);
      // self.emit("error", serviceInstance, err);
    });
  });
};

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

    // checking first attempt when there is no active socket (listing services)
    if (!client.isOpen()) {
      self.emit('error', client.service, new Error("There is no active connection"));
      return;
    }

    client.status(buffer);

  } catch (e) {
    this.emit("error", serviceInstance, e)
  }
};

TcpManager.prototype.connect = function(serviceInstance) {
  var self = this;
  return new Promise(function(resolve, reject) {
    try {
      var client = _getClient(serviceInstance);

      client.connect().then(function() {
        // preparing socket listeners
        self.initialize(client);

      }).catch(function(err) {
        console.log(err)
      }).finally(function() {
        resolve()
        // self.emit('serviceConnected', serviceInstance);
      })
    } catch (e) {
      reject(e);
      // this.emit("error", serviceInstance, e);
    }
  });
};

TcpManager.prototype.getService = function(serviceInstance) {
  return _getClient(serviceInstance);
};

// async
TcpManager.prototype.stopService = function(serviceInstance) {
  var self = this;
  try {
    var buffer = self.makebuffer(Signals.TERMINATE_SERVICE_SIGNAL, {});

    var client = _getClient(serviceInstance);

    self.initialize(client);

    client.stop(buffer);

  } catch(e) {
    this.emit("error", e);
  }

};

TcpManager.prototype.registerListeners = function (serviceInstance) {
  var self = this;
  if (serviceInstance) {
    var client = _getClient(serviceInstance);

    self.initialize(client);
    return;
  }

  Object.keys(clients).forEach(function(key) {
    self.initialize(client);
  })
};

TcpManager.prototype.initialize = function(client) {
  var self = this;
  if (!client.isOpen() || client.isRegistered())
    return;

  console.log("Registering listeners " + client.service.name);
  this.registered = true;

  var onStatus = function(response) {
    self.emit('statusReceived', client.service, response)
  };

  var onLog = function(response) {
    // TODO: make a local buffer
    var target = [];
    switch(client.service.service_type_id) {
      case ServiceType.COLLECTOR:
        target = logs.collectors;
        break;
      case ServiceType.ANALYSIS:
        target = logs.analysis;
        break;
    }

    if (target.length === 0) {
      target.push(response);
    } else {
      target.forEach(function(cachedLog) {
        // cachedLog.process_id
        response.some(function(logRetrieved) {
          if (cachedLog.process_id === logRetrieved.process_id) {
            cachedLog.log.push.apply(logRetrieved.log);
            return true;
          }
        })
      });
    }

    self.emit('logReceived', client.service, response)
  };

  var onStop = function(response) {
    self.emit('stop', client.service, response);
  };

  var onClose = function(response) {
    self.emit('close', client.service, response);
  };

  var onError = function(response) {
    self.emit('error', client.service, response)
  };

  client.on('status', onStatus);

  client.on('log', onLog);

  // terminate signal
  client.on('stop', onStop);

  // socket closed
  client.on('close', onClose);

  client.on('error', onError)

  // remove listener
  self.on('removeListeners', function() {
    console.log("Removing listener from " + client.service.name);

    client.removeListener('status', onStatus);
    client.removeListener('log', onLog);
    client.removeListener('stop', onStop);
    client.removeListener('close', onClose);
    client.removeListener('error', onError);

    self.registered = false;
  })
};

TcpManager.prototype.isServiceConnected = function(serviceInstance) {
  var client = _getClient(serviceInstance);
  return client.isOpen();
}

TcpManager.prototype.disconnect = function() {
  // disabling listeners
  for(var k in clients) {
    if (clients.hasOwnProperty(k)) {
      clients[k].socket.destroy();
      console.log(clients[k].service.name + " socket destroyed");
    }
  }
};

module.exports = new TcpManager();
