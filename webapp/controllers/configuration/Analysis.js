var Enums = require('../../core/Enums');

module.exports = function(app) {
  return {
    "index": function analysesController(request, response) {
      response.render("configuration/analyses");
    },
    "new": function newAnalyseController(request, response) {
      response.render("configuration/analysis", { Enums: Enums });
    }
  };
};
