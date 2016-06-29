var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var TokenCode = require('./../../core/Enums').TokenCode;
var passport = require('./../../config/Passport');
var TcpManagerClass = require('./../../core/TcpManager');

var TcpManager = new TcpManagerClass();

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

    post: [passport.isAdministrator, function(request, response) {
      var serviceObject = request.body.service;
      serviceObject.log = request.body.log;
      DataManager.addServiceInstance(serviceObject).then(function(service) {
        var token = Utils.generateToken(app, TokenCode.SAVE, service.name);
        console.log(token);
        return response.json({status: 200, token: token});
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      });
    }],

    put: [passport.isAdministrator, function(request, response) {
      var serviceId = request.params.id;
      var serviceObject = request.body.service;
      serviceObject.log = request.body.log;
      DataManager.updateServiceInstance(serviceId, serviceObject).then(function() {
        DataManager.getServiceInstance({id: serviceId}).then(function(serviceInstance) {
          var token = Utils.generateToken(app, TokenCode.UPDATE, serviceInstance.name);

          if (TcpManager.isServiceConnected(serviceInstance)) {
            try {
              TcpManager.emit('updateService', serviceInstance);

              setTimeout(function() {
                // shutdown
                TcpManager.emit('stopService', serviceInstance);
              }, 2000)
            } catch(e) {
              console.log(e);
            }
          }

          // TODO: send update signal and restart service
          return response.json({status: 200, token: token});
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        });
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      });
    }],

    delete: [passport.isAdministrator, function(request, response) {
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
    }]
  };
};
