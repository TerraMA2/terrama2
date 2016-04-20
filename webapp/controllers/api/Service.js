var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");


module.exports = function(app) {
  return {
    get: function(request, response) {
      DataManager.listServiceInstances().then(function(services) {
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