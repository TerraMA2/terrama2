"use strict";

var DataManager = require('./../../core/DataManager');
var Enums = require('./../../core/Enums');

module.exports = function(app) {
  return function(request, response) {
    var _redirectToHome = function(err) {
      console.log(err);
      response.redirect("/");
    };

    DataManager.listCollectors().then(function(collectors) {
      DataManager.listAnalyses().then(function(analysisList) {
        var outputAnalysis = [];
        analysisList.forEach(function(analysis) {
          outputAnalysis.push(analysis.rawObject());
        });

        var outputCollectors = [];
        collectors.forEach(function(collector) {
          outputCollectors.push(collector.rawObject());
        });

        return response.render("configuration/status", {
          "Enums": Enums,
          "analysis": outputAnalysis,
          "collectors": outputCollectors
        });
      }).catch(_redirectToHome);
    }).catch(_redirectToHome);
  };
};
