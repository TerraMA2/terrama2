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

    var onStatusReceivedData = function(service, response){
      var flagObject = dataSentFlags[service.id];
      if (flagObject && !flagObject.isDataSent) {
        console.log("sendind data");
        Utils.prepareAddSignalMessage(DataManager).then(function(data) {
          TcpManager.emit('sendData', service, data);
        });

        flagObject.isDataSent = true;
      } else {
        console.log("not sending");
      }
    };

    var onStatusReceived = function(service, response) {
      client.emit('statusResponse', {
        status: 200,
        service: service.id,
        loading: false,
        online: Object.keys(response).length > 0
      });
    };

    var onLogReceived = function(service, response) {
      console.log("RECEBEU");
      client.emit('logResponse', {
        status: 200,
        logs: response,
        service_type: service.service_type_id
      });
    };

    var onStop = function(service, response) {
      client.emit('stopResponse', {
        status: 200,
        online: false,
        loading: false,
        service: service.id
      })
    };

    var onClose = function(service, response) {
      console.log(response);
      client.emit('closeResponse', {
        status: 400,
        service: service.id,
        loading: false,
        online: false
      })
    };

    var onError = function(service, err) {
      client.emit('errorResponse', {
        status: 400,
        message: err.toString(),
        service: service ? service.id : 0
      })
    };

    var dataSentFlags = {};

    // register listeners
    DataManager.listServiceInstances().then(function(instances) {
      instances.forEach(function(instance) {
        TcpManager.registerListeners(instance);
      })
    });

    // tcp listeners
    TcpManager.on('serviceStarted', onServiceStarted);

    TcpManager.on('serviceConnected', onServiceConnected);

    TcpManager.on('statusReceived', onStatusReceivedData);

    TcpManager.on('statusReceived', onStatusReceived);

    TcpManager.on('logReceived', onLogReceived);

    TcpManager.on('stop', onStop);

    TcpManager.on('close', onClose);

    TcpManager.on('error', onError);

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
        dataSentFlags[instance.id] = { id: instance.id, isDataSent: false };

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
              TcpManager.updateService(instance);

              setTimeout(function() {
                TcpManager.emit('statusService', instance);
              }, 2000);
            }).catch(_handleErr)
          }, 2000);
        }).catch(_handleErr);
      }).catch(function(err) {
        console.log(err);
      })
    });
    // end client start listener

    client.on('status', function(json) {
      DataManager.getServiceInstance({id: json.service}).then(function(instance) {
        // notify every one with loading
        iosocket.emit('statusResponse', {
          status: 200,
          loading: true,
          online: false,
          service: instance.id
        });

        TcpManager.emit('statusService', instance);
      }).catch(function(err) {
        console.log(err);
        client.emit('statusResponse', {
          status: 400,
          online: false,
          loading: false,
          service: json.service
        })
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
          service: service.id
        })
      });
    });

    client.on('log', function(json) {
      var begin = json.begin || 0,
          end = json.end || 2;

      var _handleError = function(err) {
        console.log(err);
        client.emit('errorResponse', {
          status: 400,
          message: err.toString()
        })
      };

      DataManager.listServiceInstances().then(function(services) {
        DataManager.listAnalyses().then(function(analysisList) {
          var obj = {
            begin: begin,
            end: end
          };

          DataManager.listCollectors().then(function(collectors) {
            var analysisIds = analysisList.map(function(element) { return element.id });
            var collectorsIds = collectors.map(function(elm) { return elm.id });
            services.forEach(function(service) {
              switch(service.service_type_id) {
                case ServiceType.ANALYSIS:
                obj.process_ids = analysisIds;
                  // requesting for analysis log
                  TcpManager.emit('logData', service, obj);
                  break;
                case ServiceType.COLLECTOR:
                  obj.process_ids = collectorsIds,

                  // requesting for collector log
                  TcpManager.emit('logData', service, obj);
                  break;
                default:
                  _handleError(new Error("Invalid service type"));
              }
            })
          }).catch(_handleError)
        }).catch(_handleError)
      }).catch(_handleError)
    });

    client.on('disconnect', function() {
      // // removing clients listeners of TcpManager instance
      // TcpManager.emit('removeListeners');

      // removing tcp listener
      TcpManager.removeListener('statusReceived', onStatusReceived);
      TcpManager.removeListener('statusReceived', onStatusReceivedData);
      TcpManager.removeListener('logReceived', onLogReceived);
      TcpManager.removeListener('stop', onStop);
      TcpManager.removeListener('close', onClose);
      TcpManager.removeListener('error', onError);
      TcpManager.removeListener('serviceStarted', onServiceStarted);
      TcpManager.removeListener('serviceConnected', onServiceConnected);

      console.log("DISCONNECTED");
    })
  });
};

module.exports = TcpSocket;
