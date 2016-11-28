"use strict";

/**
 * NodeJS event
 * @type {Event.EventEmitter}
 */
var EventEmitter = require("events").EventEmitter;

/**
 * TerraMA² logger library
 * @type {winston.Logger}
 */
var logger = require("./../../Logger");

/**
 * It defines a Promiser module used in TerraMA² application
 * @type {bluebird.Promise}
 */
var PromiseClass = require("./../../Promise");

// TerraMA2 Utils
var Utils = require("./../../Utils");

// TcpManager
var TcpManager = require("./../../TcpManager");

// TerraMA2 Enums
var ServiceType = require("./../../Enums").ServiceType;

// DataManager
var DataManager = require("./../../DataManager");

// TerraMA² Service model
var Service = require("./../../data-model/Service");

// TerraMA² RegisteredView model
var RegisteredView = require("./../../data-model/RegisteredView");

// TerraMA² App Metadata
var Application = require("./../../Application");

/**
 * It defines TerraMA² webapp metadata (package.json)
 * @type {Object}
 */
var webapp = Application.get("metadata");

/**
 * It handles TCP service manipulation. Use  it to be pipe between front-end and back-end application
 * 
 * @class TcpService
 * @emits #serviceStarting When a service is ready to start. Useful for notify all listeners. Remember that it does not represent that service will be executed successfully.
 * @emits #serviceStatus When user request for service status in order to determines if service is running properly
 * @emits #serviceRequesting Status When user is requesting for status. Useful to notify all listeners
 * @emits #serviceVersion When TerraMA² retrieves a version of C++ services
 */
function TcpService() {
  EventEmitter.apply(this, arguments);

  /**
   * Define if TcpService is already initialized
   * @type {boolean}
   */
  this.$loaded = false;

  /**
   * Define a list of services already registered with listeners
   * @type {number[]}
   * @private
   */
  var _registeredServices = [];

  /**
   * It register a service with listener and append it into registered service list
   * 
   * @param {Service} service - TerraMA² Service instance
   * @returns {Promise} When finishes
   */
  this.register = function(service) {
    return new PromiseClass(function(resolve, reject) {
      // If no service given, reject the promise
      if (!service) {
        return reject(new Error("No service to register"));
      }

      // seek by a service in registered services list
      var found = _registeredServices.find(function(element) { return element === service.id; });
      // If service in cache list, reject with exception
      if (found) {
        return reject(new Error(Utils.format("Service %s is already registered.", service.id)));
      }
      // register listeners
      TcpManager.registerListeners(service);
      // register cached service
      _registeredServices.push(service.id);
      // ensure promise resolves
      return resolve();
    });
  };

  /**
   * Retrieve cached list of registered services
   * @returns {number[]}
   */
  this.$getRegisteredServices = function() {
    return _registeredServices;
  };

  /**
   * It resets registered services cache
   */
  this.$setRegisteredServices = function(arr) {
    _registeredServices = arr;
  };
}
// Javascript inheritance way
TcpService.prototype = Object.create(EventEmitter.prototype);
TcpService.prototype.constructor = TcpService;

/**
 * It initializes service listeners based on database service
 * 
 * @param {boolean} shouldConnect - Flag to determine if tries to connect on running service
 * @returns {Promise}
 */
TcpService.prototype.init = function(shouldConnect) {
  var self = this;
  return new PromiseClass(function(resolve) {
    // register listeners
    return DataManager.listServiceInstances()
      .then(function(instances) {
        // TODO: throw exception
        var promises = [];
        if (!self.$loaded) {
          // registering tcp manager listener
          TcpManager.on("statusReceived", onStatusReceived);
          TcpManager.on("logReceived", onLogReceived);
          TcpManager.on("stop", onStop);
          TcpManager.on("close", onClose);
          TcpManager.on("tcpError", onError);
          TcpManager.on("processFinished", onProcessFinished);
          TcpManager.on("serviceVersion", onServiceVersionReceived);

          self.$loaded = true;
          instances.forEach(function(instance) {
            // register cache
            self.register(instance);
            // tries to connect automatically on running services
            promises.push(self.$sendStatus(instance));
          });
        }

        return Promise.all(promises)
          .then(function() {
            return resolve();
          })
          .catch(function(err) {
            return resolve();
          });
      });
  });
};

/**
 * It finalizes TcpService session, resetting registered services cache and removing TcpManager listeners
 * 
 * @returns {Promise}
 */
TcpService.prototype.finalize = function() {
  var self = this;
  return new PromiseClass(function(resolve) {
    if (self.$loaded) {
      // remove TcpManager listener
      TcpManager.removeListener("statusReceived", onStatusReceived);
      TcpManager.removeListener("logReceived", onLogReceived);
      TcpManager.removeListener("stop", onStop);
      TcpManager.removeListener("close", onClose);
      TcpManager.removeListener("tcpError", onError);
      TcpManager.removeListener("processFinished", onProcessFinished);
      TcpManager.removeListener("serviceVersion", onServiceVersionReceived);
      self.$loaded = false;
    }
    // resetting cache
    self.$setRegisteredServices([]);

    return resolve();
  });
};

/**
 * Listener for handling client start request. When called,
 * it retrieves a service instance and tries to start TerraMA² service executable.
 * Once success during start, it performs a socket connection using Service configurations.
 * After that, it sends status service signal to communicate
 * 
 * @param {Object} json - A given arguments sent by client
 * @param {number} json.service - A TerraMA² service instance id
 */
TcpService.prototype.start = function(json) {
  /**
   * @type {TcpService}
   */
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    return DataManager.getServiceInstance({id: json.service})
      .then(function(instance) {
        // emitting service starting
        self.emit("serviceStarting", {service: instance.id});

        // spreading all promises in order to retrieve service instance and promise result in two vars
        return Promise.all([instance, TcpManager.startService(instance)]);
      })
      // on sucess, pass the service and code execution value
      .spread(function(service, exitCode) {
        if (exitCode !== 0) {
          throw new Error(Utils.format("Not executed successfully. Exit Code: %s", exitCode));
        }

        /**
         * It defines how many times NodeJS tried to connect in service
         * @type {number}
         */
        var times = 0;
        
        /**
         * It handles service connection. By default, when OS starts, the virtual memory is too low. It turns out
         * low performance during process execution. In this case, the TerraMA² wont initialize properly. We tried to increase timeout, 
         * but sometimes it occurs. In order to avoid it, we try to connect three times with default interval. 
         * If connection success, resolve promise chain. Otherwise, break recursion, rejecting promise error chain.
         * @returns {Promise}
         */
        var connectionRepeat = function() {
          // delay to start service. We are not able to detect if service is already running, 
          // since it depends OS calls. Ok, exit code is 0, but we must ensure that is available
          return PromiseClass.delay(3000)
            .then(function() {
              return TcpManager.connect(service)
                .then(function() {
                  return TcpManager.statusService(service);
                })
                .catch(function(err) {
                  // once tried three times, throw err in order to continue promise chain
                  if (times === 3) {
                    logger.error(Utils.format("TerraMA² Service %s is not running.", service.name));
                    throw err;
                  }
                  times += 1;
                  logger.warn(Utils.format("Failed to connect %s. Retrying... %s", service.name, times));
                  // auto call
                  return connectionRepeat();
                });
            });
        };

        return connectionRepeat();
      })
      // on sucess starting, resolve promise
      .then(function() {
        return resolve();
      })
      // on any error
      .catch(function(err) {
        var exception = new Error(Utils.format("Error occurred during start service %s. %s", json.service, err.toString()));
        logger.error(exception);
        // emits exception before reject promise
        self.emit("serviceError", {
          exception: exception,
          message: exception.toString(),
          service: json.service  
        });
        return reject(exception);
      });
  });
}; // end client start listener

/**
 * Listener for handling start process signal. When it called, it tries to send START_PROCESS signal
 * in order to forcing a process to execute in TerraMA².
 * 
 * @param {Object} processObject - A given arguments sent by client
 * @param {number} processObject.service_instance - A TerraMA² service instance id
 * @returns {Promise}
 */
TcpService.prototype.run = function(processObject) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    /**
     * Service id to run
     * @type {number}
     */
    var service = processObject.service_instance;
    // remove service id from object
    delete processObject.service_instance;
    return DataManager.getServiceInstance({id: service})
      .then(function(instance) {
        TcpManager.startProcess(instance, processObject);
        self.emit("processRun", processObject);
        return resolve();
      })
      .catch(function(err) {
        logger.debug(err);
        return reject(err);
      });
  });
}; // end run

/**
 * It performs service status through TcpManager.
 * 
 * @private
 * @param {Service} service - TerraMA² service instance to send 
 * @returns {Promise}
 */
TcpService.prototype.$sendStatus = function(service) {
  /**
   * Current TcpService reference
   * @type {TcpService}
   */
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    var params = {
      status: 200,
      checking: true,
      online: false,
      service: service.id
    };
    // notify every one with loading
    self.emit("serviceRequestingStatus", params);

    return TcpManager.connect(service)
      .then(function() {
        TcpManager.emit("statusService", service);
        return resolve();
      })
      .catch(function(err) {
        params.status = 400;
        params.checking = false;
        // notify every one
        self.emit("serviceRequestingStatus", params);
        return reject(err);
      });
  });
};

/**
 * Listener for handling status signal. When it called, it tries to connect to the socket and retrieve a
 * life time using STATUS_SIGNAL.
 * 
 * @param {Object} json - A given arguments sent by client
 * @param {number} json.service - A TerraMA² service instance id
 */
TcpService.prototype.status = function(json) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    return DataManager.getServiceInstance({id: json.service})
      .then(function(instance) {
        return self.$sendStatus(instance);
      })

      .then(function() {
        return resolve();
      })

      }).catch(function(err) {
        logger.debug(err);
        self.emit("serviceError", {
          exception: err,
          message: err.toString(),
          service: json.service
        });
        return reject(err);
      });
}; // end client status listener

/**
 * Listener for handling STOP service signal. When called, it sends a STOP_SERVICE signal followed by a STATUS_SERVICE.
 * Once TerraMA² executable receives STOP_SERVICE, it starts changing shutdown the running active processes, so it may
 * take a few seconds/minutes to finish. 
 * 
 * @param {Object} json - A given arguments sent by client
 * @param {number} json.service - A TerraMA² service instance id
 */
TcpService.prototype.stop = function(json) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    return DataManager.getServiceInstance({id: json.service})
      .then(function(instance) {
        self.emit("serviceStopping", instance);

        TcpManager.emit("stopService", instance);
        TcpManager.statusService(instance);

        return resolve();
      }).catch(function(err) {
        logger.debug(err);
        self.emit("serviceError", {
          exception: err,
          message: err.toString(),
          service: json.service
        });
        return reject(err);
      });
  });
}; // end client stop listener

/**
 * Listener for handling data to be sent to C++ services. It does not emits signal.
 * 
 * @param {Object} data - A given arguments sent by client
 * @param {Analysis[]} data.Analysis - A list of Analysis to send
 * @param {Collectors[]} data.Collectors - A list of Collectors to send
 * @param {Views[]} data.Views - A list of Views to send
 * @param {DataSeries[]} data.DataSeries - A list of DataSeries to send
 * @param {DataProviders[]} data.DataProviders - A list of DataProviders to send
 * @param {number} service - A TerraMA² service instance to send. When typed, it send only for this.
 */
TcpService.prototype.send = function(data, serviceId) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    /**
     * Target promiser to expose Database Retriever. It might be listServiceInstances or getServiceInstance
     * @type {Promise<Service|Services[]>}
     */
    var promiser;
    if (serviceId) {
      promiser = DataManager.getServiceInstance({id: serviceId});
    } else {
      promiser = DataManager.listServiceInstances();
    }

    return promiser
      .then(function(services) {
        var _sendData = function(service) {
          try {
            TcpManager.emit('sendData', service, data);
          } catch(err) { }
        };
        if (services instanceof Array) { // listServiceInstances
          services.forEach(function(service) {
            _sendData(service);
          });
        } else { // getServiceInstance
          _sendData(services);
        }
        return resolve();
      })
      .catch(function(err) {
        return reject(err);
      });
  });
};

/**
 * Listener for remove data from C++ services. It does not emits signal even exception.
 * @param {Object} data - A given arguments sent by client
 * @param {Analysis[]} data.Analysis - A list of Analysis to send
 * @param {Collectors[]} data.Collectors - A list of Collectors to send
 * @param {Views[]} data.Views - A list of Views to send
 * @param {DataSeries[]} data.DataSeries - A list of DataSeries to send
 * @param {DataProviders[]} data.DataProviders - A list of DataProviders to send
 * @param {number} service - A TerraMA² service instance to send. When typed, it send only for this.
 */
TcpService.prototype.remove = function(data, serviceId) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    return DataManager.listServiceInstances()
      .then(function(services) {
        services.forEach(function (service) {
          try {
            TcpManager.emit('removeData', service, data);
          } catch (e) {
            logger.debug(e);
          }
        });
        return resolve();
      })
      .catch(function(err) {
        logger.debug(err);
        return reject(err);
      });
  });
};

/**
 * Listener for handling Log request signal. When called, it maps the cached logs and if necessary request
 * for others through LOG_SIGNAL in socket communication.
 * 
 * @param {Object} json - A given arguments sent by client
 * @param {number} json.begin - A begin offset to retrieve logs
 * @param {number} json.end - An end offset to retrieve logs
 */
TcpService.prototype.log = function(json) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    /**
     * Defines a interval to retrieve logs. From begin (0) to end (2)
     * @type {number}
     */
    var begin = json.begin || 0,
          end = json.end || 2;

    return PromiseClass.all([
        DataManager.listServiceInstances(),
        DataManager.listAnalysis(),
        DataManager.listCollectors(),
        DataManager.listViews()
      ])
      // spreading promiser result into services, analysisList, collectors and views variables
      .spread(function(services, analysisList, collectors, views) {
        var obj = {
          begin: begin,
          end: end
        };

        if (services.length === 0) {
          // throwing to promiser catcher
          throw new Error("No service available");
        }

        services.forEach(function(service) {
          switch(service.service_type_id) {
            case ServiceType.ANALYSIS:
              obj.process_ids = analysisList.map(function(element) { return element.id; });
              break;
            case ServiceType.COLLECTOR:
              obj.process_ids = collectors.map(function(elm) { return elm.id; });
              break;
            case ServiceType.VIEW:
              obj.process_ids = views.map(function(elm) { return elm.id; });
              break;
            default:
              throw new Error("Invalid service type");
          }

          // requesting log data
          TcpManager.emit("logData", service, obj);
        }); // end foreach
      }) // end spread
      .then(function() {
        return resolve();
      })

      .catch(function(err) {
        return reject(err);
      });
  }); // end promise
}; // end log listener

/**
 * It destroys opened sockets
 */
TcpService.prototype.disconnect = function() {
  TcpManager.disconnect();
};

/**
 * TcpService Singleton. It will be exported
 * @type {TcpService}
 */
var tcpService = new TcpService();

/**
 * Listener for handling TerraMA² TcpManager Service Status
 * 
 * @emits #serviceStatus When TerraMA² C++ Service is already loaded
 * @emits #statusService When TerraMA² C++ Service is not loaded yet
 * @param {Service} service - TerraMA² service
 * @param {Object} response - Response object
 * @param {boolean} response.service_loaded - Identifier if service was loaded before
 * @param {boolean} response.shutting_down - Identifier if service is shutting down
 */
function onStatusReceived(service, response) {
  if (!response.service_loaded) {
    // send updateService and Data
    TcpManager.updateService(service);

    setTimeout(function() {
      return Utils.prepareAddSignalMessage(DataManager).then(function(data) {
        TcpManager.emit("sendData", service, data);
      }).finally(function() {
        // checking status again
        TcpManager.emit("statusService", service);
      });
    }, 1000);
  } else {
    tcpService.emit("serviceStatus", {
      status: 200,
      service: service.id,
      shutting_down: response.shutting_down,
      loading: false,
      online: Object.keys(response).length > 0
    });
  }
}

/**
 * It handles service version retrieved during TerraMA² C++ service initialization. It emits #serviceVersion with service and response
 * 
 * @param {Service} service - TerraMA² service
 * @param {string} response - Response version
 */
function onServiceVersionReceived(service, response) {
  var version = webapp.version;
  tcpService.emit("serviceVersion", {
    service: service.id,
    response: response.replace("TerraMA2", ""),
    current: version,
    match: response.endsWith(version)
  });
}

/**
 * It emits a signal depending Process. 
 * If service is View, it emits #viewReceived with the registered view object.
 * 
 * @param {Object|RegisteredView} resp - any object
 * @returns {void}
 */
function onProcessFinished(resp) {
  // broadcast to everyone
  if (resp instanceof RegisteredView) {
    tcpService.emit("viewReceived", resp);
  } else {
    tcpService.emit("processFinished", resp);
  }
}

/**
 * Listener for handling TerraMA² TcpManager Log values
 *
 * @emits #serviceLog 
 * @param {Service} service - TerraMA² service
 * @param {Object} response - Response object with log values from respective service
 */
function onLogReceived(service, response) {
  tcpService.emit("serviceLog", {
    status: 200,
    logs: response,
    service_type: service.service_type_id,
    service: service.name
  });
}

/**
 * Listener for handling Service Stopping. Once called, it will emit object determining service is stopping, 
 * but you must call STATUS to ensure service is stopped.
 * 
 * @emits #serviceStop
 * @param {Service} service - TerraMA² Service
 */
function onStop(service) {
  tcpService.emit("serviceStop", {
    status: 200,
    online: false,
    loading: false,
    service: service.id
  });
}

/**
 * Listener for handling socket connection close.
 * 
 * @emits #serviceStop
 * @param {Service} service - TerraMA² Service
 * @param {Object} response - Socket Response Object
 */
function onClose(service, response) {
  logger.debug(response);
  tcpService.emit("serviceStop", {
    status: 400,
    service: service.id,
    loading: false,
    online: false
  });
}

/**
 * Listener for handlign any error during socket communication.
 * 
 * @emits #serviceError
 * @param {Service} service - TerraMA² Service
 * @param {Error} err - Socket error
 */
function onError(service, err) {
  tcpService.emit("serviceError", {
    status: 400,
    message: err.toString(),
    service: service ? service.id : 0
  });
}

module.exports = tcpService;
