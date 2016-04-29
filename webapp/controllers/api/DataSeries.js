var DataManager = require("../../core/DataManager");
var TcpManager = require('../../core/TcpManager');
var Utils = require("../../core/Utils");
var DataSeriesError = require('../../core/Exceptions').DataSeriesError;
var Intent = require('./../../core/Enums').DataProviderIntent;
var isEmpty = require('lodash').isEmpty;

module.exports = function(app) {
  return {
    post: function(request, response) {
      var dataSeriesObject = request.body.dataSeries;
      var scheduleObject = request.body.schedule;
      var filterObject = request.body.filter;
      var serviceId = request.body.service;

      if (dataSeriesObject.hasOwnProperty('input') && dataSeriesObject.hasOwnProperty('output')) {
        // getting service instance (analysis)
        DataManager.getServiceInstance({service_type_id: serviceId}).then(function(serviceResult) {
          DataManager.addDataSeriesAndCollector(dataSeriesObject, scheduleObject, filterObject, serviceResult).then(function(dataSeriesResult) {
            // sending tcp data
            var output = [];
            dataSeriesResult.forEach(function(dSeries) {
              output.push(dSeries.toObject());
            });

            TcpManager.sendData({'DataSeries': output});
            return response.json(output);
          }).catch(function(err) {
            return Utils.handleRequestError(response, err, 400);
          });
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      } else {
        DataManager.addDataSeries(dataSeriesObject).then(function(dataSeriesResult) {
          if (!isEmpty(filterObject)) {
            DataManager.addFilter(filterObject).then(function(filterResult) {
              response.json({status: 200});
            }).catch(function(err) {
              return Utils.handleRequestError(response, err, 400);
            });
          } else
            response.json({status: 200});
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      }
    },

    get: function(request, response) {
      var dataSeriesId = request.params.id;
      var dataSeriesType = request.query.type;
      
      // collector scope
      var collector = request.query['collector'];
      
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
      
      if (collector) {
        restriction.Collector = {};
      }

      if (dataSeriesId) {

        DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeries) {
          return response.json(dataSeries.toObject());
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      } else {
        DataManager.listDataSeries(restriction).then(function(dataSeriesList) {
          var output = [];
          dataSeriesList.forEach(function(dataSeries) {
            output.push(dataSeries.toObject());
          });
          response.json(output);
        }).catch(function(err) {
          console.log(err);
          return Utils.handleRequestError(response, err, 400);
        });
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