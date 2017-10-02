module.exports = function(app) {
  'use strict';

  var DataManager = require("./../../core/DataManager");
  var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

  var controllers = {
    new: function(request, response) {
      response.render("configuration/legend");
    },
    edit: function(request, response) {
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      DataManager.getLegend({id: parseInt(request.params.id)}).then(function(legend) {
        return response.render("configuration/legend", {legend: legend.rawObject(), hasProjectPermission: hasProjectPermission});
      }).catch(function(err) {
        return response.render("base/404");
      });
    }
  }
  return controllers;
};