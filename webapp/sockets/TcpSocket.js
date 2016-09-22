"use strict";

/**
 * Socket responsible for handling Tcp events from TcpManager in front end application.
 * @class TcpSocket
 *
 * @author Raphael Willian da Costa
 *
 * @property {object} iosocket - Sockets object.
 * @property {object} memberScriptChecker - Script checker class.
 */
var TcpSocket = function(io) {

  // Sockets object
  var iosocket = io.sockets;

  // TcpManager
  var TcpManager = require('./../core/TcpManager');

  // TerraMA2 Utils
  var Utils = require('./../core/Utils');

  // TerraMA2 Enums
  var ServiceType = require('./../core/Enums').ServiceType;

  // DataManager
  var DataManager = require('./../core/DataManager');

  // Socket connection event
  iosocket.on('connection', function(client) {

    console.log("NEW socket.io CONNECTION");

    var onServiceStarted = function(service) {
      setTimeout(function() {
        TcpManager.emit('connect', service);
      }, 2000);
    };

    var onServiceConnected = function(service) {
      TcpManager.emit('updateService', service);

      setTimeout(function() {
        TcpManager.emit('statusService', service);
      }, 2000);
    };

    var onStatusReceived = function(service, response) {
      if (!response.service_loaded) {
        // send updateService and Data
        TcpManager.updateService(service);

        setTimeout(function() {
          Utils.prepareAddSignalMessage(DataManager).then(function(data) {
            TcpManager.emit('sendData', service, data);
          }).finally(function() {
            // checking status again
            TcpManager.emit("statusService", service);
          });
        }, 1000);
      } else {
        client.emit('statusResponse', {
          status: 200,
          service: service.id,
          loading: false,
          online: Object.keys(response).length > 0
        });
      }
    };

    var onLogReceived = function(service, response) {
      client.emit('logResponse', {
        status: 200,
        logs: response,
        service_type: service.service_type_id,
        service: service.name
      });
    };

    var onStop = function(service) {
      client.emit('stopResponse', {
        status: 200,
        online: false,
        loading: false,
        service: service.id
      });
    };

    var onClose = function(service, response) {
      console.log(response);
      client.emit('closeResponse', {
        status: 400,
        service: service.id,
        loading: false,
        online: false
      });
    };

    var onError = function(service, err) {
      client.emit('errorResponse', {
        status: 400,
        message: err.toString(),
        service: service ? service.id : 0
      });
    };

    // register listeners
    DataManager.listServiceInstances().then(function(instances) {
      instances.forEach(function(instance) {
        TcpManager.registerListeners(instance);
      });
    });

    // tcp listeners
    TcpManager.on('serviceStarted', onServiceStarted);

    TcpManager.on('serviceConnected', onServiceConnected);

    TcpManager.on('statusReceived', onStatusReceived);

    TcpManager.on('logReceived', onLogReceived);

    TcpManager.on('stop', onStop);

    TcpManager.on('close', onClose);

    TcpManager.on('tcpError', onError);

    // client listeners
    client.on('start', function(json) {
      DataManager.getServiceInstance({id: json.service}).then(function(instance) {
        var _handleErr = function(err) {
          client.emit('errorResponse', {
            status: 400,
            message: err.toString(),
            service: instance ? instance.id : 0
          });
        };

        // notify every one with loading
        iosocket.emit('statusResponse', {
          status: 200,
          loading: true,
          service: instance.id
        });

        TcpManager.startService(instance).then(function(code) {
          if (code !== 0) {
            _handleErr(new Error("Error occurred during start service. " + code));
            return;
          }
          setTimeout(function() {
            TcpManager.connect(instance).then(function() {
              TcpManager.statusService(instance);
            }).catch(_handleErr);
          }, 3000);
        }).catch(_handleErr);
      }).catch(function(err) {
        console.log(err);
      });
    });
    // end client start listener

    client.on('run', function(process_object){
      var service_instance = process_object.service_instance;
      delete process_object.service_instance;
      DataManager.getServiceInstance({id: service_instance}).then(function(instance) {
        TcpManager.startProcess(instance, process_object);
        client.emit('runResponse', process_object);
      }).catch(function(err) {
        console.log(err);
      });
    });

    client.on('status', function(json) {
      /**
       * Helper for handling error callbacks. It notifies client listeners.
       * 
       * @param {Error} err - A threw callback error
       */
      var _emitError = function(err) {
        client.emit('statusResponse', {
          status: 400,
          online: false,
          message: err.toString(),
          loading: false,
          service: json.service
        });
      };

      DataManager.getServiceInstance({id: json.service}).then(function(instance) {
        // notify every one with loading
        iosocket.emit('statusResponse', {
          status: 200,
          loading: true,
          online: false,
          service: instance.id
        });

        TcpManager.connect(instance).then(function() {
          TcpManager.emit('statusService', instance);
        }).catch(_emitError);
      }).catch(function(err) {
        console.log(err);
        client.emit('statusResponse', {
          status: 400,
          online: false,
          loading: false,
          service: json.service
        });
      });
    });
    // end client status listener

    client.on('stop', function(json) {
      DataManager.getServiceInstance({id: json.service}).then(function(instance) {
        iosocket.emit('stopResponse', {
          status: 200,
          loading: true,
          online: true,
          service: instance.id
        });

        TcpManager.emit('stopService', instance);
      }).catch(function(err) {
        console.log(err);
        client.emit('errorResponse', {
          status: 400,
          message: err.toString(),
          service: json.service.id
        });
      });
    }); // end client stop listener

    client.on('log', function(json) {
      var begin = json.begin || 0,
          end = json.end || 2;

      var _handleError = function(err) {
        console.log(err);
        client.emit('errorResponse', {
          status: 400,
          message: err.toString()
        });
      };

      DataManager.listServiceInstances().then(function(services) {
        DataManager.listAnalyses().then(function(analysisList) {
          var obj = {
            begin: begin,
            end: end
          };

          if (services.length === 0 && analysisList.length === 0) {
            _handleError(new Error("No service available"));
            return;
          }

          DataManager.listCollectors().then(function(collectors) {
            var analysisIds = analysisList.map(function(element) { return element.id; });
            var collectorsIds = collectors.map(function(elm) { return elm.id; });
            services.forEach(function(service) {
              switch(service.service_type_id) {
                case ServiceType.ANALYSIS:
                obj.process_ids = analysisIds;
                  // requesting for analysis log
                  TcpManager.emit('logData', service, obj);
                  break;
                case ServiceType.COLLECTOR:
                  obj.process_ids = collectorsIds;

                  // requesting for collector log
                  TcpManager.emit('logData', service, obj);
                  break;
                default:
                  _handleError(new Error("Invalid service type"));
              }
            });
          }).catch(_handleError);
        }).catch(_handleError);
      }).catch(_handleError);
    }); // end log listener

    client.on('disconnect', function() {
      // removing clients listeners of TcpManager instance
      TcpManager.removeListener('statusReceived', onStatusReceived);
      TcpManager.removeListener('logReceived', onLogReceived);
      TcpManager.removeListener('stop', onStop);
      TcpManager.removeListener('close', onClose);
      TcpManager.removeListener('tcpError', onError);
      TcpManager.removeListener('serviceStarted', onServiceStarted);
      TcpManager.removeListener('serviceConnected', onServiceConnected);

      console.log("DISCONNECTED");
    });
  });
};

module.exports = TcpSocket;
