"use strict";

var Enums = require('../../core/Enums');
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
var DataManager = require('./../../core/DataManager');

module.exports = function(app) {
  return {
    "get": function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      parameters.hasProjectPermission = hasProjectPermission;
      response.render("configuration/analysis_list", Object.assign({"Enums": Enums}, parameters));
    },
    "new": function(request, response) {
      response.render("configuration/analysis", { Enums: Enums, projectId: request.session.activeProject.id });
    },

    edit: function(request, response) {
      var analysisId = request.params.id;
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;

      DataManager.getAnalysis({ id: analysisId }).then(function(analysisResult) {
        response.render("configuration/analysis", {
          Enums: Enums,
          analysis: analysisResult.rawObject(),
          projectId: request.session.activeProject.id,
          hasProjectPermission: hasProjectPermission
        });
      }).catch(function(err) {
        response.render("base/404");
      });
    }
  };
};
