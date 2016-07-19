var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var TokenCode = require('./../../core/Enums').TokenCode;
var TcpManager = require('./../../core/TcpManager');
var Log = require('./../../core/data-model/').Log;
module.exports = function(app) {
  return {
    get: function(request, response) {
      var type = request.query.type;
      var serviceId = request.query.serviceId;

      if (!serviceId) {
        //todo: improve it
        var restriction = {};
        switch (type) {
          case "COLLECT":
            restriction = {service_type_id: 1};
            break;
          case "ANALYSIS":
            restriction = {service_type_id: 2};
            break;
          default:
            break;
        }

        DataManager.listServiceInstances(restriction).then(function(services) {
          // todo: checking status - on/off
          var output = [];
          services.forEach(function(service) {
            output.push(service.rawObject());
          })
          return response.json(output);
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        })
      } else {
        DataManager.getServiceInstance({id: serviceId}).then(function(service) {
          response.json({status: 200, result: service.rawObject()});
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        })
      }
    },

    post: function(request, response) {
      var serviceObject = request.body.service;
      serviceObject.log = request.body.log;
      DataManager.addServiceInstance(serviceObject).then(function(service) {
        var token = Utils.generateToken(app, TokenCode.SAVE, service.name);
        console.log(token);
        return response.json({status: 200, token: token});
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      });
    },

    put: function(request, response) {
      var serviceId = request.params.id;
      var serviceObject = request.body.service;

      var _handleError = function(err) {
        Utils.handleRequestError(response, err, 400);
      };

      serviceObject.log = request.body.log;
      DataManager.getServiceInstance({id: serviceId}).then(function(serviceInstance) {
        var shouldRestart = !Utils.equal({
          runEnviroment: serviceInstance.runEnviroment,
          port: serviceInstance.port,
          sshUser: serviceInstance.sshUser,
          pathToBinary: serviceInstance.pathToBinary,
          host: serviceInstance.host
        }, {
          runEnviroment: serviceObject.runEnviroment,
          port: serviceObject.port,
          sshUser: serviceObject.sshUser,
          pathToBinary: serviceObject.pathToBinary,
          host: serviceObject.host
        });

        var logSent = new Log(serviceObject.log);
        var logInDatabase = serviceInstance.log;

        DataManager.updateServiceInstance(serviceId, serviceObject).then(function() {
          var _continueRequest = function() {
            DataManager.getServiceInstance({id: serviceInstance.id}).then(function(newServiceInstance) {
              var token = Utils.generateToken(app, TokenCode.UPDATE, newServiceInstance.name);

              if (TcpManager.isServiceConnected(serviceInstance)) {
                try {
                  console.log("Should restart? - " + shouldRestart);
                  if (shouldRestart) {
                    TcpManager.emit('stopService', serviceInstance);
                  } else
                    TcpManager.emit('updateService', serviceInstance);
                } catch(e) {
                  console.log(e);
                }
              } else
                shouldRestart = false;
              return response.json({status: 200, token: token, service: serviceInstance.id, restart: shouldRestart});
            });
          };

          if (!Utils.equal(logSent.toObject(), logInDatabase.toObject())) {
            shouldRestart = true;
            DataManager.updateLog(serviceInstance.log.id, logSent).then(function() {
              _continueRequest();
            }).catch(_handleError);
          } else
            _continueRequest();
        }).catch(_handleError);
      }).catch(_handleError);
    },

    delete: function(request, response) {
      var serviceId = request.params.id;
      DataManager.getServiceInstance({id: serviceId}).then(function(serviceInstance) {
        DataManager.removeServiceInstance({id: serviceId}).then(function() {
          return response.json({status: 200, name: serviceInstance.name});
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        });
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      })
    }
  };
};
