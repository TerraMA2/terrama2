var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");


module.exports = function(app) {
  return {
    get: function(request, response) {
      // todo: implement it
      response.json([]);
    },

    post: function(request, response) {
      var analysisObject = request.body;

      // if not has project_id, getting from cache
      if (analysisObject.project_id)
        analysisObject.project_id = app.locals.activeProject.id;

      DataManager.addAnalysis(analysisObject).then(function(analysisResult) {
        console.log(analysisResult);
        response.json({status: 200});
      }).catch(function(err) {
        console.log(err);
        Utils.handleRequestError(response, err, 400);
      })
    }
  };
};