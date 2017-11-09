var DataManager = require("./../../core/DataManager");
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
var Application = require("./../../core/Application");

module.exports = function(app) {
  return {
    get: function (request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      var settings = {
        service: request.query.service,
        restart: request.query.restart
      };
      return response.render('administration/services', Object.assign(settings, parameters));
    },

    new: function (request, response) {
      var configFile = Application.getContextConfig();
      return response.render('administration/service', {db: configFile.db});
    },
    
    edit: function(request, response) {
      DataManager.getServiceInstance({id: request.params.id}).then(function(service) {
        return response.render('administration/service', {service: service.rawObject()});
      }).catch(function(err) {
        response.json({status: 400, message: err.message});
      })
    }
  };
};