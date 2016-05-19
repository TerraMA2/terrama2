var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");


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
        return response.json({status: 200});
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      });
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
