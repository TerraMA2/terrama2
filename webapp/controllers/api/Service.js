"use strict";

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

        return DataManager.listServiceInstances(restriction).then(function(services) {
          var output = [];
          services.forEach(function(service) {
            output.push(service.rawObject());
          });
          return response.json(output);
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      } else {
        return DataManager.getServiceInstance({id: serviceId}).then(function(service) {
          return response.json({status: 200, result: service.rawObject()});
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      }
    },

    post: function(request, response) {
      var serviceObject = request.body.service;
      serviceObject.log = request.body.log;
      return DataManager.addServiceInstance(serviceObject).then(function(service) {
        var token = Utils.generateToken(app, TokenCode.SAVE, service.name);
        return response.json({status: 200, token: token});
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });
    },

    put: function(request, response) {
      var serviceId = request.params.id;
      var serviceObject = request.body.service;

      var _handleError = function(err) {
        Utils.handleRequestError(response, err, 400);
      };

      serviceObject.log = request.body.log;
      /**
       * Flag to handle if service should restart
       * @type {boolean}
       */
      var shouldRestart;
      var serviceInstance;

      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};

        return DataManager.getServiceInstance({id: serviceId}, options)
          .then(function(serviceInstanceResult) {
            // setting closure variable
            serviceInstance = serviceInstanceResult;

            shouldRestart = !Utils.equal({
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

            return DataManager.updateServiceInstance(serviceId, serviceObject, options)
              .then(function() {
                if (!Utils.equal(logSent.toObject(), logInDatabase.toObject())) {
                  shouldRestart = true;
                  return DataManager.updateLog(serviceInstance.log.id, logSent, options);
                } else {
                  return null;
                }
              }); // end DataManager.updateServiceInstance(serviceId, serviceObject)
          })

          .then(function() {
            return DataManager.getServiceInstance({id: serviceInstance.id}, options);
          });
      })
      // on commit
      .then(function(newServiceInstance) {
        var token = Utils.generateToken(app, TokenCode.UPDATE, newServiceInstance.name);
        if (TcpManager.isServiceConnected(serviceInstance)) {
          try {
            if (shouldRestart) {
              TcpManager.emit('stopService', serviceInstance);
            } else {
              TcpManager.emit('updateService', serviceInstance);
            }
          } catch(e) {
          }
        } else { shouldRestart = false; }
        return response.json({status: 200, token: token, service: serviceInstance.id, restart: shouldRestart});
      })
      // on rollback
      .catch(_handleError);
    },

    delete: function(request, response) {
      var serviceId = request.params.id;
      return DataManager.getServiceInstance({id: serviceId}).then(function(serviceInstance) {
        return DataManager.removeServiceInstance({id: serviceId}).then(function() {
          // stopping service
          TcpManager.stopService(serviceInstance);
          return response.json({status: 200, name: serviceInstance.name});
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });
    }
  };
};
