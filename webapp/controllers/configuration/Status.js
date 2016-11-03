"use strict";

var DataManager = require('./../../core/DataManager');
var Enums = require('./../../core/Enums');
var PromiseModule = require("./../../core/Promise");

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

        return response.render("configuration/status", {
          "Enums": Enums,
          "analysis": outputAnalysis,
          "collectors": outputCollectors,
          "views": outputViews
        });
      })

      .catch(function(err) {
        console.log(err);
        return response.redirect("/");
      });
  };
};
