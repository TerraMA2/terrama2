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
const { MessageType, ServiceType, StatusLog, Uri } = require('./../../Enums');

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
var versionData = require("../../../../share/terrama2/version.json");

const loadDatabaseFunctions = require('./../../utility/loadDatabaseFunctions');

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
 * @returns {Promise}
 */
TcpService.prototype.init = async function() {
  try {
    // register listeners
    const instances = await DataManager.listServiceInstances()
    // TODO: throw exception
    var promises = [];
    if (!this.$loaded) {
      // registering tcp manager listener
      TcpManager.on("statusReceived", onStatusReceived);
      TcpManager.on("logReceived", onLogReceived);
      TcpManager.on("stop", onStop);
      TcpManager.on("close", onClose);
      TcpManager.on("tcpError", onError);
      TcpManager.on("processFinished", onProcessFinished);
      TcpManager.on("serviceVersion", onServiceVersionReceived);
      TcpManager.on("processValidated", onProcessValidated);
      TcpManager.on("notifyView", onNotifyView);

      this.$loaded = true;

      for(let instance of instances) {
        // register cache
        this.register(instance);
        // tries to connect automatically on running services
        promises.push(this.$sendStatus(instance));
      }
    }

    await PromiseClass.all(promises);
  } finally {
    return null;
  }
};

/**
 * It finalizes TcpService session, resetting registered services cache and removing TcpManager listeners
 *
 * @returns {Promise}
 */
TcpService.prototype.finalize = async function() {
  if (this.$loaded) {
    // remove TcpManager listener
    TcpManager.removeListener("statusReceived", onStatusReceived);
    TcpManager.removeListener("logReceived", onLogReceived);
    TcpManager.removeListener("stop", onStop);
    TcpManager.removeListener("close", onClose);
    TcpManager.removeListener("tcpError", onError);
    TcpManager.removeListener("processFinished", onProcessFinished);
    TcpManager.removeListener("serviceVersion", onServiceVersionReceived);
    TcpManager.removeListener("processValidated", onProcessValidated);
    TcpManager.removeListener("notifyView", onNotifyView);
    this.$loaded = false;
  }
  // resetting cache
  this.$setRegisteredServices([]);

  return null;
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
TcpService.prototype.start = async function(json) {
  let instance = null;
  try {
    instance = await DataManager.getServiceInstance({id: json.service})
    // emitting service starting
    this.emit("serviceStarting", {service: instance.id});

    // spreading all promises in order to retrieve service instance and promise result in two vars
    const [ service, exitCode ] = await PromiseClass.all([instance, TcpManager.startService(instance)]);

    if (exitCode !== 0) {
      throw new Error(Utils.format("Not executed successfully. Exit Code: %s", exitCode));
    }

    /**
     * It defines how many times NodeJS tried to connect in service
     * @type {number}
     */
    let times = 0;
    /**
     * It handles service connection. By default, when OS starts, the virtual memory is too low. It turns out
     * low performance during process execution. In this case, the TerraMA² wont initialize properly. We tried to increase timeout,
     * but sometimes it occurs. In order to avoid it, we try to connect three times with default interval.
     * If connection success, resolve promise chain. Otherwise, break recursion, rejecting promise error chain.
     * @returns {Promise}
     */
    const connectionRepeat = () => (
      // delay to start service. We are not able to detect if service is already running,
      // since it depends OS calls. Ok, exit code is 0, but we must ensure that is available
      delay(3000)
        .then(() => TcpManager.connect(service))
        .then(() => TcpManager.statusService(service))
        .catch(err => {
          // once tried three times, throw err in order to continue promise chain
          if (times === 3) {
            logger.error(Utils.format("TerraMA² Service %s is not running.", service.name));
            throw err;
          }
          times += 1;
          logger.warn(Utils.format("Failed to connect %s. Retrying... %s", service.name, times));
          // auto call
          return connectionRepeat();
        })
    );
    // on sucess starting, resolve promise
    return await connectionRepeat();
  } catch (err) {
    var exception = new Error(`Error occurred during the service "${instance ? instance.name : ''}" startup. ${err.message}`);
    logger.error(exception);
    // emits exception before reject promise
    this.emit("serviceError", {
      exception: exception,
      message: exception.toString(),
      service: json.service
    });
    throw exception;
  }
}; // end client start listener

/**
 * Listener for handling start process signal. When it called, it tries to send START_PROCESS signal
 * in order to forcing a process to execute in TerraMA².
 *
 * @param {Object} processObject - A given arguments sent by client
 * @param {number} processObject.service_instance - A TerraMA² service instance id
 * @returns {Promise}
 */
TcpService.prototype.run = async function(processObject) {
  /**
   * Service id to run
   * @type {number}
   */
  const service = processObject.service_instance;
  // remove service id from object
  delete processObject.service_instance;
  let instance = await DataManager.getServiceInstance({id: service});
  // If alert service, check if have a view to start first
  if (instance.service_type_id == ServiceType.ALERT){
    const alert = await DataManager.getAlert({id: processObject.ids[0]});
    // if alert has view, start the view process
    if (alert.view && alert.view.id) {
      const view = await DataManager.getView({id: alert.view.id});
      instance = await DataManager.getServiceInstance({id: view.serviceInstanceId});

      processObject = {
        execution_date: processObject.execution_date,
        ids: [ view.id ]
      };
    }
  }

  startProcess(instance, processObject);
  // Notify children listeners the process has been scheduled
  this.emit("processRun", processObject);

  return null;
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

TcpService.prototype.stopAll = function stopAll() {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    return DataManager.listServiceInstances()
      .then(function(instances) {
        instances.forEach(function(instance) {
          self.emit("serviceStopping", instance);

          TcpManager.emit("stopService", instance);
        });

        return resolve();
      }).catch(function(err) {
        logger.debug(err);
        self.emit("serviceError", {
          exception: err,
          message: err.toString(),
        });
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
}; // end client status listener

/**
 * Listener for handling status signal when try to delete an object.
 *
 * @param {Object} json - A given arguments sent by client
 * @param {number} json.service - A TerraMA² service instance id
 */
TcpService.prototype.statusToDelete = function(json) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    return DataManager.getServiceInstance({id: json.service})
      .then(function(instance) {
        return TcpManager.connect(instance)
          .then(function() {
            var params = {
              online: true,
              service: instance
            };
            return resolve(params);
          })
          .catch(function() {
            var params = {
              online: false,
              service: instance
            };
            return resolve(params);
          });
      });
  });
}; // end client status to delete listener

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
TcpService.prototype.send = async function(data, serviceId) {
  let services = [];

  if (serviceId) {
    const service = await DataManager.getServiceInstance({ id: serviceId });
    services.push(service);
  } else {
    services = await DataManager.listServiceInstances();
  }

  for(let service of services) {
    TcpManager.emit('sendData', service, data);
  }

  return;
};

/**
 * Emits a given event sending a given object.
 *
 * @param {String} event - Event to be emitted
 * @param {Object} data - Data to be sent
 */
TcpService.prototype.emitEvent = function(event, data) {
  tcpService.emit(event, data);
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
        DataManager.listViews(),
        DataManager.listAlerts(),
        DataManager.listInterpolators()
      ])
      // spreading promiser result into services, analysisList, collectors and views variables
      .spread(function(services, analysisList, collectors, views, alerts, interpolators) {
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
            case ServiceType.ALERT:
              obj.process_ids = alerts.map(function(elm) { return elm.id; });
              break;
            case ServiceType.INTERPOLATION:
              obj.process_ids = interpolators.map(function(elm) { return elm.id; });
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
 * It performs TerraMA² validate process in C++ services
 *
 * @param {Object} data - A data to send via Tcp
 * @param {number} serviceId - TerraMA² Service identifier
 * @return {Promise}
 */
TcpService.prototype.validateProcess = function(data, serviceId) {
  return DataManager.getServiceInstance({id: serviceId})
    .then(function(service) {
      return TcpManager.validateProcess(service, data);
    });
};

/**
 * It destroys opened sockets
 */
TcpService.prototype.disconnect = function() {
  TcpManager.disconnect();
};

/**
 * Send signal to monitor remove a layer
 *
 * @param {Object} registeredView - View info to remove
 * @return {Promise}
 */
TcpService.prototype.removeView = function(registeredView){
  var self = this;
  var viewObject = {
    workspace: registeredView.workspace,
    layer: registeredView.layers[0],
    parent: registeredView.dataSeriesType,
    private: registeredView.view.private
  }
  self.emit("removeView", viewObject);
}

/**
 * TcpService Singleton. It will be exported
 * @type {TcpService}
 */

const tcpService = new TcpService();

/**
 * Delay a promise handling for time provided
 *
 * @param {number} milliseconds Number of milliseconds to delay
 * @return {Promise}
 */
function delay(milliseconds) {
  return PromiseClass.delay(milliseconds);
};

/**
 * This method send status signal to the TerraMA² service
 *
 * Due complexity of wait for TerraMA² status signal, this method just
 * sends status pack every second to help display
 *
 * @todo Once TerraMA² Services wait signal, this method will be deprecated
 *
 * @param {Service} service TerraMA² Service instance
 * @param {number} times Number of times to repeat
 * @returns {Promise}
 */
const repeatStatus = (service, times) => {
  // Setting default time
  if (!times) {
    times = 10;
  }

  const serviceClient = TcpManager.getService(service);

  // Wrapper of repeat promiser
  const _repeat = () => {
    // When done, finish promise chain
    if (--times < 1) {
      return Promise.resolve();
    }
    // When Service is offline, abort
    // It seems to be an error
    if (!serviceClient.isOpen()) {
      return Promise.reject(new Error(`No connection`));
    }
    // Wait a second and then dispatches status signal
    return delay(1000)
      .then(() => TcpManager.emit("statusService", service))
      .then(() => _repeat());
  }

  return _repeat();
};

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
async function onStatusReceived(service, response) {
  if (!response.service_loaded) {
    // send updateService and Data
    TcpManager.updateService(service);

    const { user, host, password, database, port } = service.log;
    const logURI = `postgis://${user}:${password}@${host}:${port}/${database}`;
    await loadDatabaseFunctions(logURI);

    delay(1000)
      .then(() => Utils.prepareAddSignalMessage(DataManager))
      .then(data => TcpManager.emit("sendData", service, data))
      .finally(() => repeatStatus(service, 20))
      .catch(err => {
        logger.error(`Error while checking TerraMA² status: ${err.toString()}`)
      });

  } else {
    tcpService.emit("serviceStatus", {
      status: 200,
      service: service.id,
      shutting_down: response.shutting_down,
      loading: false,
      online: Object.keys(response).length > 0,
      start_time: response.start_time,
      terrama2_version: response.terrama2_version,
      web_version: versionData.major + "." + versionData.minor + "." + versionData.patch + "-" + versionData.tag,
      logger_online: response.logger_online,
      maps_server_connection: response.maps_server_connection
    });
  }
}

/**
 * It handles TerraMA² Process validation retrieved from C++ Services. Once received, it notifies all TcpService listeners.
 * It must not broadcast everyone in TcpSocket, since it is independent of each user
 *
 * Listener for handling TerraMA² TcpManager Service Status
 *
 * @emits #serviceStatus When TerraMA² C++ Service is already loaded
 * @emits #statusService When TerraMA² C++ Service is not loaded yet
 * @param {Service} service - TerraMA² service
 * @param {Object} response - Response object. TODO: doc it
 */
function onProcessValidated(service, response) {
  // TODO: emits only the necessary and not everything
  tcpService.emit("processValidated", response || service);
}

/**
 * It handles service version retrieved during TerraMA² C++ service initialization. It emits #serviceVersion with service and response
 *
 * @param {Service} service - TerraMA² service
 * @param {string} response - Response version
 */
function onServiceVersionReceived(service, response) {
  var version = versionData.major + "." + versionData.minor + "." + versionData.patch + "-" + versionData.tag;//webapp.version;
  tcpService.emit("serviceVersion", {
    service: service.id,
    response: response.replace("TerraMA2", "").toLowerCase(),
    current: version.toLowerCase(),
    match: response.toLowerCase().endsWith(version.toLowerCase())
  });
}

/**
 * It emits a signal depending Process.
 * If service is View, it emits #viewReceived with the registered view object.
 *
 * @param {Object} resp - Process Object
 * @param {any} resp.process - Process Metadata
 * @param {RegisteredView?} resp.view - TerraMA² Registered View
 * @returns {void}
 */
function onProcessFinished(resp) {
  // broadcast to everyone
  if (resp && resp instanceof RegisteredView) {
    tcpService.emit("viewReceived", resp);
    // check if is a view from alert to execute the alert process
    DataManager.getAlert({view_id: resp.view.id})
      .then(function(alert){
        DataManager.getServiceInstance({id: alert.service_instance_id}).then(function(instance){
          var alertProcessObject = {
            ids: [alert.id],
            execution_date: new Date().toISOString()
          };
          startProcess(instance, alertProcessObject);
          // Notify children listeners the process has been scheduled
          tcpService.emit("processRun", alertProcessObject);
        })
      })
      .catch(function(){
        logger.debug("Dont have alert of the view");
      });
  }
  // Notifies that process finished
  tcpService.emit("processFinished", resp.process);
}

/**
 * It emits a signal to nofity webMonitor.
 * If service has a view, it emits #notifyView with the registered view object.
 *
 * @param {Object} resp - Process Object
 * @param {RegisteredView?} resp.registeredView - TerraMA² Registered View
 * @returns {void}
 */
function onNotifyView(resp) {
  if (resp.registeredView){
    var viewObject = {
      workspace: resp.registeredView.workspace,
      layer: resp.registeredView.layers[0],
      private: resp.registeredView.view.private
    };
    tcpService.emit("notifyView", viewObject);
  }
}

/**
 * It emits a signal to nofity webMonitor to remove a layer.
 *
 * @param {Object} resp - Process Object
 * @param {RegisteredView?} resp.registeredView - TerraMA² Registered View
 * @returns {void}
 */
function RemoveView(registeredView){
  var viewObject = {
    workspace: registeredView.workspace,
    layer: registeredView.layers[0],
    private: registeredView.view.private
  }
  tcpService.emit("removeView", viewObject);
}

/**
 * Start process
 * @param {*} instance
 * @param {*} processObject
 */
function startProcess(instance, processObject){
  // Forcing a process to run
  TcpManager.startProcess(instance, processObject);
  // Retrieving log status of process (STARTED/ON_QUEUED, etc)
  TcpManager.logData(instance, {begin: 0, end: 2, process_ids: processObject.ids});
}

/**
 * Listener for handling TerraMA² TcpManager Log values
 * If the service is View and log is done, create a link to geoserver layer created
 *
 * @emits #serviceLog
 * @param {Service} service - TerraMA² service
 * @param {Object} response - Response object with log values from respective service
 */
function onLogReceived(service, response) {
  // checking the service type
  if (service.service_type_id === ServiceType.VIEW){
    // searching list of registered views to match with response log
    DataManager.listRegisteredViews()
      .then(function(registeredViews){
        // checking if had registered views
        if (registeredViews.length > 0){
          response.forEach(function(resp){
            registeredViews.forEach(function(regView){
              if (regView.view.id === resp.process_id){
                resp.log.forEach(function(logMessage){
                  if (logMessage.status === StatusLog.DONE){
                    var link = createGeoserverLink(regView);
                    var description = "Layer link: " + (regView.layers.length > 0 ? regView.layers[0].name : "");
                    logMessage.messages.push({link: link, type: MessageType.LINK_MESSAGE, description: description});
                  }
                });
              }
            });
          });
        }
        tcpService.emit("serviceLog", {
          status: 200,
          logs: response,
          service_type: service.service_type_id,
          service: service.name,
          length: response.length
        });
      });
  } else {
    tcpService.emit("serviceLog", {
      status: 200,
      logs: response,
      service_type: service.service_type_id,
      service: service.name,
      length: response.length
    });
  }
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

/**
 * Function to create the link to geoserver layer
 *
 * @param registeredView - Registered View with server information
 * @returns link - link to server layer
 */
function createGeoserverLink(registeredView){
  var layerName = (registeredView.layers.length > 0 ? registeredView.layers[0].name : "");
  var workspace = registeredView.workspace;
  var protocol = registeredView.uri.split('//')[0];
  var baseLink = registeredView.uri.split("@")[1];
  var link = protocol + '//' + baseLink + "/web/?wicket:bookmarkablePage=:org.geoserver.web.data.resource.ResourceConfigurationPage&name=" + layerName + "&wsName=" + workspace;
  return link;
}

module.exports = tcpService;