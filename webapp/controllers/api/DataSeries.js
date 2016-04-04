var DataManager = require("../../core/DataManager");
var Utils = require("../../core/Utils");

module.exports = function(app) {
  return {
    post: function(request, response) {
      var dataSeriesObject = request.body;

      DataManager.addDataSeries(dataSeriesObject).then(function(dataSeriesResult) {
        return response.json(dataSeriesResult);
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });

    },

    get: function(request, response) {

    },

    put: function(request, response) {

    }
  };
};