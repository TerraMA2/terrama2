"use strict";

var DataManager = require("../../core/DataManager");

module.exports = function(app) {
  return {
    get: function(request, response) {
      var project = request.params.project;

      if(project) {
        var output = [];
        DataManager.listCollectors({ DataProvider: { project_id: project } }).then(function(collectors) {
          collectors.forEach(function(element) {
            output.push(element.rawObject());
          });
          response.json(output);
        });
      } else {
        var output = [];
        DataManager.listDataProviders({}).then(function(collectors) {
          collectors.forEach(function(element) {
            output.push(element.rawObject());
          });
          response.json(output);
        });
      }
    }
  };
};
