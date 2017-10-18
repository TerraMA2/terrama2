"use strict";

var Signals = require('./Signals.js');
var logger = require("./Logger");
var Process = require('./Process');
var Executor = require('./executors/Local');
var SSH = require("./executors/SSH");
var Utils = require('./Utils');
var _ = require('lodash');
var Service = require('./Service');
var NodeUtils = require('util');
var EventEmitter = require('events').EventEmitter;
var ServiceType = require('./Enums').ServiceType;
var PromiseClass = require('./Promise');
var Application = require('./Application');

// Facades
var ProcessFinished = require("./facade/tcp-manager/ProcessFinished");

/**
 * It handles entire TCP communication between TerraMA² NodeJS application and C++ Services.
 * Do not use it directly. Prefer to use TcpService.js
 * 
 * @inherits EventEmitter
 * @class TcpManager
 */
var TcpManager = module.exports = function() {
  EventEmitter.call(this);
  // registering self listeners
  this.on('startService', this.startService);
  this.on('stopService', this.stopService);
  this.on('sendData', this.sendData);
  this.on('removeData', this.removeData);
  this.on('logData', this.logData);
  this.on('statusService', this.statusService);
  this.on('updateService', this.updateService);
  this.on('connect', this.connect);
  this.on("validateProcess", this.validateProcess);

  this.registered = false;
};
// NodeJs helper to make inherited class
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
    if(isNaN(signal)) { throw TypeError(signal + " is not a valid signal!"); }

    var totalSize;
    var jsonMessage = "";

    var hasMessage = !_.isEmpty(object);

    if (hasMessage) {
      jsonMessage = JSON.stringify(object).replace(/\":/g, "\": ");

      // The size of the message plus the size of two integers, 4 bytes each
      totalSize = jsonMessage.length + 4;
    } else { totalSize = 4; }

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
    if (bufferMessage.length > jsonMessage.length) { totalSize = bufferMessage.length + 4; }

    // Writes the buffer size (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(totalSize, 0);

    // // Writes the signal (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(signal, 4);

    return buffer;
  } catch (e) {
    throw e;
  }
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
      if (clients[key].service.name === connection.name) {
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
  analysis: [],
  views: []
};

/**
 * Base method to write data in TCP stream
 * 
 * @param {ServiceInstance} serviceInstance - A TerraMA² service instance
 * @param {Object} data - Data to make buffer
 * @param {Signals} signal - A TCP signal to communicate 
 */
TcpManager.prototype.$send = function(serviceInstance, data, signal) {
  try {
    var client = _getClient(serviceInstance);
    var buffer = this.makebuffer(signal, data);
    logger.debug("BufferToString: ", buffer.toString());
    logger.debug("BufferToString size: ", buffer.length);

    client.send(buffer);
  } catch (e) {
    this.emit('tcpError', serviceInstance, new Error("Could not send data to service", e));
  }
};

/**
 * This method sends a ADD_DATA_SIGNAL with bytearray to tcp socket. It is async
 * 
 * @param {ServiceInstance} serviceInstance - a terrama2 service instance
 * @param {Object} data - a javascript object message to send
 */
TcpManager.prototype.sendData = function(serviceInstance, data) {
  this.$send(serviceInstance, data, Signals.ADD_DATA_SIGNAL);
};

/**
 * This method send a START_PROCESS_SIGNAL with json process ids.
 * 
 * @param {ServiceInstance} serviceInstance - a terrama2 service instance
 * @param {Json} data - A Json with type and process ids
 */
TcpManager.prototype.startProcess = function(serviceInstance, data){
  this.$send(serviceInstance, data, Signals.START_PROCESS_SIGNAL);
};

/**
 * This method sends a REMOVE_DATA_SIGNAL with bytearray to tcp socket. It is async
 * 
 * @param {ServiceInstance} serviceInstance - a terrama2 service instance
 * @param {Object} data - a javascript object message to send
 */
TcpManager.prototype.removeData = function(serviceInstance, data) {
  this.$send(serviceInstance, data, Signals.REMOVE_DATA_SIGNAL);
};

/**
 * This method sends a LOG_SIGNAL with bytearray to tcp socket. It is async
 * 
 * @param {ServiceInstance} serviceInstance - a terrama2 service instance
 * @param {Object} data - a javascript object message to send
 * @param {number} data.begin - Begin interval to retrieve
 * @param {number} data.end - End interval to retrieve
 */
TcpManager.prototype.logData = function(serviceInstance, data) {
  var self = this;
  try {
    // checking if there are active connections
    if (Object.keys(clients).length === 0) {
      throw new Error("There is no client to log request. Start services in Admin Dashboard");
    }

    var client = _getClient(serviceInstance);

    // checking first attempt when there is no active socket (listing services)
    if (!client.isOpen()) {
      self.emit('error', client.service, new Error("There is no active connection"));
      return;
    }

    var buffer = self.makebuffer(Signals.LOG_SIGNAL, data);
    // requesting for log
    client.log(buffer);

  } catch (e) {
    this.emit('tcpError', serviceInstance, new Error("Could not send data LOG_SIGNAL to service", e));
  }
};

/**
 * This method sends a UPDATE_SERVICE_SIGNAL with service instance values to tcp socket.
 * 
 * @param {ServiceInstance} serviceInstance - a terrama2 service instance
 */
TcpManager.prototype.updateService = function(serviceInstance) {
  this.$send(serviceInstance, serviceInstance.toObject(), Signals.UPDATE_SERVICE_SIGNAL);
};

/**
 * This method sends a VALIDATE_PROCESS_SIGNAL to specific service in order to validate Process value
 * 
 * @param {Service} serviceInstance - TerraMA² Service
 * @param {Object} data - Process object to send
 */
TcpManager.prototype.validateProcess = function(serviceInstance, data) {
  this.$send(serviceInstance, data, Signals.VALIDATE_PROCESS_SIGNAL);
};

/**
 * This method connects via ssh to service host and sends terminal command to start service.
 * 
 * @param {ServiceInstance} serviceInstance - a terrama2 service instance
 * @return {PromiseClass} a bluebird PromiseClass
 */
TcpManager.prototype.startService = function(serviceInstance) {
  var self = this;

  return new PromiseClass(function(resolve, reject) {
    var instance = new Process();
    if (serviceInstance.host && serviceInstance.host !== "") {
      instance.setAdapter(new SSH());
    } else {
      instance.setAdapter(new Executor());
    }

    return instance.connect(serviceInstance).then(function() {
      return PromiseClass.all([
        instance.adapter.execute(serviceInstance.pathToBinary, ['--version'], {}),
        instance.startService()
      ]).spread(function(versionResponse, startResponse) {
        self.emit("serviceVersion", serviceInstance, versionResponse.data);

        return resolve(startResponse.code);
      }).catch(function(err) {
        var config = Application.getContextConfig();
        var tryDefaultPath = (serviceInstance.pathToBinary === config.defaultExecutableName);

        if(tryDefaultPath) {
          serviceInstance.pathToBinary = (config.defaultExecutablePath.endsWith("/") ? config.defaultExecutablePath : config.defaultExecutablePath + "/") + config.defaultExecutableName;

          return PromiseClass.all([
            instance.adapter.execute(serviceInstance.pathToBinary, ['--version'], {}),
            instance.startService()
          ]).spread(function(versionResponse, startResponse) {
            self.emit("serviceVersion", serviceInstance, versionResponse.data);

            return resolve(startResponse.code);
          }).catch(function(err) {
            return reject(err);
          }).finally(function() {
            return instance.disconnect();
          });
        } else {
          return reject(err);
        }
      }).finally(function() {
        return instance.disconnect();
      });
    }).catch(function(err) {
      return reject(err);
    });
  });
};

/**
 This method sends STATUS_SIGNAL and waits for tcp client response.
 @param {ServiceInstance} serviceInstance - a terrama2 service instance
 @return {PromiseClass} a bluebird PromiseClass
 */
TcpManager.prototype.statusService = function(serviceInstance) {
  var self = this;
  try {
    var buffer = self.makebuffer(Signals.STATUS_SIGNAL, {});

    var client = _getClient(serviceInstance);

    // checking first attempt when there is no active socket (listing services)
    if (!client.isOpen()) {
      self.emit('error', client.service, new Error("There is no active connection"));
      return;
    }

    client.status(buffer);

  } catch (e) {
    this.emit('tcpError', serviceInstance, e);
  }
};

TcpManager.prototype.connect = function(serviceInstance) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    try {
      var client = _getClient(serviceInstance);

      if (client.isOpen()) {
        return resolve();
      }

      return client.connect().then(function() {
        // preparing socket listeners
        self.initialize(client);
        return resolve();
      }).catch(function(err) {
        return reject(err);
      });
    } catch (e) {
      return reject(e);
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
    this.emit('tcpError', e);
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
  });
};

TcpManager.prototype.initialize = function(client) {
  var self = this;
  if (!client.isOpen() || client.isRegistered()) { return; }

  logger.debug("Registering listeners " + client.service.name);
  this.registered = true;

  var onStatus = function(response) {
    self.emit('statusReceived', client.service, response);
  };
  //receive c++ logs
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
      case ServiceType.VIEW:
        target = logs.views;
        break;
    }

    if (target.length === 0) {
      Array.prototype.push.apply(target, response);
    } else {
      target.forEach(function(cachedLog) {
        // cachedLog.process_id
        response.some(function(logRetrieved) {
          if (cachedLog.process_id === logRetrieved.process_id) {
            cachedLog.log.push.apply(logRetrieved.log);
          }
        });
      });
    }

    self.emit('logReceived', client.service, response, target.length);
  };

  /**
   * It listens when Tcp service C++ execution is done
   * @param {Object} response - It contains a C++ service object response
   */
  var onProcessFinished = function(response) {
    if (Utils.isObject(response)) {
      /**
       * Retrieving last log process.
       */
      logger.debug(Utils.format("%s finished. Retrieving LOG Process finished in order to keep in cache", client.service.name));
      self.logData(client.service, {begin: 0, end: 2, process_ids: [response.process_id]});

      return ProcessFinished.handle(response)
        .then(function(targetProcess) {
          if (targetProcess){
            // if the finished process is from collector or analysis run conditioned process
            if (targetProcess.serviceType == ServiceType.COLLECTOR || targetProcess.serviceType == ServiceType.ANALYSIS){
              targetProcess.processToRun.forEach(function(processToRun){
                if (processToRun && response.automatic !== false){
                  self.startProcess(processToRun.instance, {ids: processToRun.ids, execution_date: response.execution_date});
                  self.logData(processToRun.instance, {begin: 0, end: 2, process_ids: [processToRun.ids]});
                }
              });
            }
            // if the finished process ir from view, save/update the registered view
            else if (targetProcess.serviceType == ServiceType.VIEW){
              self.emit("processFinished", targetProcess.registeredView);
            } 
            else if (targetProcess.serviceType == ServiceType.ALERT){
              if (response.notify)
                self.emit("notifyView", targetProcess);
            }
          }
        })
        
        .catch(function(err) {
          logger.warn(err);
        });
    }
  };

  /**
   * It listens when C++ TcpService finishes the process validation
   * 
   * @param {Object} response - Response object (TODO: fill all param types here)
   */
  var onValidateProcess = function(response) {
    logger.debug(Utils.format("Service %s <ValidateProcess> received %s", client.service.name, response));
    self.emit("processValidated", client.service, response);
  };

  var onStop = function(response) {
    self.emit('stop', client.service, response);
  };

  var onClose = function(response) {
    self.emit('close', client.service, response);
  };

  var onError = function(response) {
    self.emit('tcpError', client.service, response);
  };

  client.on('status', onStatus);
  client.on("validateProcess", onValidateProcess);

  client.on('log', onLog);

  // Registering On Process Finished listener
  client.on("processFinished", onProcessFinished);

  // terminate signal
  client.on('stop', onStop);

  // socket closed
  client.on('close', onClose);

  client.on('serviceError', onError);

  // remove listener
  self.on('removeListeners', function() {
    logger.debug("Removing listener from " + client.service.name);
    client.removeListener('validateProcess', onValidateProcess);
    client.removeListener('status', onStatus);
    client.removeListener('log', onLog);
    client.removeListener('stop', onStop);
    client.removeListener('close', onClose);
    client.removeListener('tcpError', onError);

    self.registered = false;
  });
};

TcpManager.prototype.isServiceConnected = function(serviceInstance) {
  var client = _getClient(serviceInstance);
  return client.isOpen();
};

TcpManager.prototype.disconnect = function() {
  // disabling listeners
  for(var k in clients) {
    if (clients.hasOwnProperty(k)) {
      clients[k].socket.destroy();
      logger.debug(clients[k].service.name + " socket destroyed");
    }
  }
};

module.exports = new TcpManager();
