module.exports = function(app) {
  'use strict';

  var DataManager = require("./../../core/DataManager");
  var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
  var config = require('../../core/Application').getContextConfig();

  var controllers = {
    get: function(request, response){
        var parameters = makeTokenParameters(request.query.token, app);
        response.render("configuration/alerts", parameters);
    },
    getLegends: function(request, response){
        var parameters = makeTokenParameters(request.query.token, app);
        parameters.legendsTab = true;
        response.render("configuration/alerts", parameters);
    },
    new: function(request, response){
        response.render("configuration/alert", { disablePDF: config.disablePDF });
    },
    edit: function(request, response) {
      DataManager.getAlert({id: parseInt(request.params.id)})
        .then(function(alert) {
          return response.render("configuration/alert", { disablePDF: config.disablePDF, alert: alert.rawObject() });
        }).catch(function(err) {
          return response.render("base/404");
        });
    }
  }
  return controllers;
};