var DataManager = require("./../../core/DataManager");

module.exports = function(app) {
  return {
    get: function (request, response) {
      return response.render('administration/services');
    },

    new: function (request, response) {
      return response.render('administration/service');
    },
    
    edit: function(request, response) {
      DataManager.getServiceInstance({id: request.params.id}).then(function(service) {
        return response.render('administration/service', {service: service});
      }).catch(function(err) {
        response.json({status: 400, message: err.message});
      })
    }
  };
};