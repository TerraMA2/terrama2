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
        DataManager.listViews()
      ])

      .spread(function(collectors, analysisList, views) {
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

        if(request.query.token !== undefined) {
          var parameters = makeTokenParameters(request.query.token, app);
          var renderParams = {
            "Enums": Enums,
            "analysis": outputAnalysis,
            "collectors": outputCollectors,
            "views": outputViews,
            "parameters": parameters
          };
        } else {
          var renderParams = {
            "Enums": Enums,
            "analysis": outputAnalysis,
            "collectors": outputCollectors,
            "views": outputViews,
            "parameters": null
          };
        }

        return response.render("configuration/status", renderParams);
      })

      .catch(function(err) {
        logger.error(err);
        return response.redirect("/");
      });
  };
};
