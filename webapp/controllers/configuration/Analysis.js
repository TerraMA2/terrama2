var Enums = require('../../core/Enums');
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
var DataManager = require('./../../core/DataManager');

module.exports = function(app) {
  return {
    "get": function analysesController(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      response.render("configuration/analyses", parameters);
    },
    "new": function newAnalyseController(request, response) {
      response.render("configuration/analysis", { Enums: Enums });
    },

    edit: function(request, response) {
      var analysisId = request.params.id;

      DataManager.getAnalysis({id: analysisId}).then(function(analysisResult) {
        response.render("configuration/analysis", {
          Enums: Enums,
          analysis: analysisResult.rawObject()
        });
      }).catch(function(err) {
        response.render("base/404");
      });
    }
  };
};
