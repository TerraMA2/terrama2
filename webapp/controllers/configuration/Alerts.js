module.exports = function(app) {
  'use strict';

  var DataManager = require("./../../core/DataManager");
  var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
  var config = require('../../core/Application').getContextConfig();

  var controllers = {
    get: function(request, response){
        var parameters = makeTokenParameters(request.query.token, app);
        var hasProjectPermission = request.session.activeProject.hasProjectPermission;
        parameters.hasProjectPermission = hasProjectPermission;
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
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      DataManager.getAlert({id: parseInt(request.params.id)})
        .then(function(alert) {
          DataManager.listAlertAttachedViews({ alert_id: alert.id })
            .then(function(alertAttachedViews) {
              DataManager.listViews({service_instance_id: alert.view.serviceInstanceId, project_id: request.session.activeProject.id})
                .then(function(views) {
                  return response.render("configuration/alert", { disablePDF: config.disablePDF, alert: alert.rawObject(), alertAttachedViews: alertAttachedViews, views: views.map(function(view) { return view.toObject(); }), hasProjectPermission: hasProjectPermission });
                }).catch(function(err) {
                  return response.render("base/404");
                });
            }).catch(function(err) {
              return response.render("base/404");
            });
        }).catch(function(err) {
          return response.render("base/404");
        });
    }
  }
  return controllers;
};