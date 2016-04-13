var DataManager = require("../../core/DataManager");
var Utils = require("../../core/Utils");
var DataSeriesError = require('../../core/Exceptions').DataSeriesError;

module.exports = function(app) {
  return {
    post: function(request, response) {
      var dataSeriesObject = request.body;

      DataManager.addDataSeries(dataSeriesObject).then(function(dataSeriesResult) {
        return response.json(dataSeriesResult.toObject());
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });

    },

    get: function(request, response) {
      var dataSeriesId = request.params.id;

      if (dataSeriesId) {
        DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeries) {
          return response.json(dataSeries.toObject());
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      } else {
        var output = [];
        DataManager.listDataSeries().forEach(function(dataSeries) {
          output.push(dataSeries.toObject());
        });
        return response.json(output);
      }
    },

    put: function(request, response) {

    }
  };
};