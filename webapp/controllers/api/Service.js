var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");


module.exports = function(app) {
  return {
    get: function(request, response) {
      var type = request.query.type;

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
        return response.json(services);
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      })
    },
    
    post: function(request, response) {
      var serviceObject = request.body;
      DataManager.addServiceInstance(serviceObject).then(function(service) {
        return response.json({status: 200});
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      });
    }
  };
};