"use strict";

var logger = require("./../../core/Logger");
var DataManager = require('./../../core/DataManager');
var Enums = require('./../../core/Enums');
var PromiseModule = require("./../../core/Promise");
var makeTokenParameters = require('./../../core/Utils').makeTokenParameters;

module.exports = function(app) {
  return function(request, response) {
    return PromiseModule
      .all([
        DataManager.listCollectors(),
        DataManager.listAnalysis(),
        DataManager.listViews(),
        DataManager.listAlerts(),
        DataManager.listInterpolators(),
        DataManager.listProjects()
      ])

      .spread(function(collectors, analysisList, views, alerts, interpolators, projects) {
        var outputAnalysis = [];
        analysisList.forEach(function(analysis) {
          outputAnalysis.push(analysis.rawObject());
        });

        var outputCollectors = [];
        collectors.forEach(function(collector) {
          outputCollectors.push(collector.rawObject());
        });

        var outputViews = views.map(function(view) {
          return view.rawObject();
        });

        var outputAlerts = alerts.map(function(alert){
          return alert.rawObject();
        });

        var outputInterpolators = interpolators.map(function(interpolator){
          return interpolator.rawObject();
        });

        var renderParams = {
          "Enums": Enums,
          "analysis": outputAnalysis,
          "collectors": outputCollectors,
          "views": outputViews,
          "alerts": outputAlerts,
          "interpolators": outputInterpolators,
          "projects": projects,
          "parameters": null
        };

        if(request.query.token !== undefined) {
          var parameters = makeTokenParameters(request.query.token, app);
          renderParams.parameters = parameters;
        }

        return response.render("configuration/status", renderParams);
      })

      .catch(function(err) {
        logger.error(err);
        return response.redirect(app.locals.BASE_URL);
      });
  };
};
