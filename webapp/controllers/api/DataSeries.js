var DataManager = require("../../core/DataManager");
var Utils = require("../../core/Utils");
var DataSeriesError = require('../../core/Exceptions').DataSeriesError;

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
      var dataSeriesId = request.params.id;

      if (dataSeriesId) {
        DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeries) {
          return response.json(dataSeries);
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      } else {
        return response.json(DataManager.listDataSeries());
      }
    },

    put: function(request, response) {

    }
  };
};