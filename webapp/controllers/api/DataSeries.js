var DataManager = require("../../core/DataManager");
var Utils = require("../../core/Utils");
var DataSeriesError = require('../../core/Exceptions').DataSeriesError;
var Intent = require('./../../core/Enums').DataProviderIntent;

module.exports = function(app) {
  return {
    post: function(request, response) {
      var dataSeriesObject = request.body.dataSeries;
      var scheduleObject = request.body.schedule;
      var filterObject = request.body.filter;
      var serviceObject = request.body.service;

      DataManager.addDataSeriesAndCollector(dataSeriesObject, scheduleObject, filterObject, serviceObject).then(function(dataSeriesResult) {
        // todo: add filter and schedule object
        return response.json(dataSeriesResult.toObject());
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });

    },

    get: function(request, response) {
      var dataSeriesId = request.params.id;
      var dataSeriesType = request.query.type;

      var dataProviderIntent;
      
      // list dataseries restriction
      var restriction = {};

      if (dataSeriesType) {
        // checking data series: static or dynamic to filter data series output
        switch(dataSeriesType) {
          case "static":
            dataProviderIntent = Intent.PROCESSING;
            break;
          case "dynamic":
            dataProviderIntent = Intent.COLLECT;
            break;
          default:
            return Utils.handleRequestError(response, new DataSeriesError("Invalid data series type. Available: \'static\' and \'dynamic\'"), 400);
        }
        
        restriction.DataProvider = {
          data_provider_intent_name: dataProviderIntent
        };
      }

      if (dataSeriesId) {

        DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeries) {
          return response.json(dataSeries.toObject());
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      } else {
        var output = [];
        DataManager.listDataSeries(restriction).forEach(function(dataSeries) {
          output.push(dataSeries.toObject());
        });
        return response.json(output);
      }
    },

    put: function(request, response) {

    },
    
    delete: function(request, response) {
      var id = request.params.id;
      
      if (id) {
        DataManager.removeDataSerie({id: id}).then(function() {
          response.json({status: 200});
        }).catch(function(err) {
          Utils.handleRequestError(response, new DataSeriesError("Dataseries not found"), 400);
        });
      } else {
        Utils.handleRequestError(response, new DataSeriesError("Missing dataseries id"), 400);
      }
    }
  };
};