var Enums = require('../../core/Enums');
var DataManager = require('./../../core/DataManager');

module.exports = function(app) {
  return {
    "index": function analysesController(request, response) {
      response.render("configuration/analyses");
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
