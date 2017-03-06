"use strict";

var DataManager = require('./../../core/DataManager');
var Enums = require('./../../core/Enums');
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
var Promise = require('bluebird');
var storedDcps = {};
var storedDcpsStore = {};


module.exports = function(app) {
  return {
    get: function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      DataManager.listCollectors().then(function(collectors){
        DataManager.listAnalysis().then(function(analysis){
          response.render('configuration/dynamicDataSeries', Object.assign({}, parameters, {"Enums": Enums, "collectors": collectors.map(
            function(element){
              return element.toObject();
            }
          ), "analysis": analysis.map(
            function(element){
              return element.rawObject();
            }
          )}));
        });
      });
    },

    new: function(request, response) {
      response.render('configuration/dataset', {type: "dynamic", "Enums": Enums});
    },

    edit: function(request, response) {
      var dataSeriesId = request.params.id;

      DataManager.getCollector({
        output: {
          id: dataSeriesId
        }
      }).then(function(collectorResult) {
        var promises = [];
        promises.push(DataManager.getDataSeries({id: collectorResult.data_series_input}));
        promises.push(DataManager.getDataSeries({id: collectorResult.data_series_output}));

        Promise.all(promises).then(function(dataSeriesResults) {
          response.render('configuration/dataset', {
            state: "dynamic",
            type: "dynamic",
            "Enums": Enums,
            dataSeries: {
              input: dataSeriesResults[0].rawObject(),
              output: dataSeriesResults[1].rawObject()
            },
            collector: collectorResult.rawObject()
          });
        });
      }).catch(function(err) {
        // check if analysis dataseries
        DataManager.getAnalysis({dataSet: {data_series_id: parseInt(dataSeriesId)}}).then(function(analysis) {
          response.redirect("/configuration/analysis/"+analysis.id+"/edit");
        }).catch(function(err) {
          // check if input dataseries (processed)
          DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeries) {
            response.render('configuration/dataset', {
              state: "dynamic",
              type: "dynamic",
              "Enums": Enums,
              dataSeries: {
                input: dataSeries.rawObject()
              }
            });
          }).catch(function(err) {
            response.render('base/404');
          });
        });
      });
    },

    storeDcps: function(request, response) {
      var key = request.body.key;
      var dcps = request.body.dcps;

      if(storedDcps[key] !== undefined)
        storedDcps[key] = storedDcps[key].concat(dcps);
      else
        storedDcps[key] = dcps;

      response.json(storedDcps[key]);
    },

    storeDcpsStore: function(request, response) {
      var key = request.body.key;
      var dcps = request.body.dcps;

      if(storedDcpsStore[key] !== undefined)
        storedDcpsStore[key] = storedDcpsStore[key].concat(dcps);
      else
        storedDcpsStore[key] = dcps;

      response.json(storedDcpsStore[key]);
    },

    paginateDcps: function(request, response) {
      var key = request.body.key;

      response.json({
        draw: parseInt(request.body.draw),
        recordsTotal: (storedDcps[key] === undefined ? 0 : storedDcps[key].length),
        recordsFiltered: (storedDcps[key] === undefined ? 0 : storedDcps[key].length),
        data: (storedDcps[key] === undefined ? [] : storedDcps[key].slice(parseInt(request.body.start), (parseInt(request.body.start) + parseInt(request.body.length))))
      });
    },

    paginateDcpsStore: function(request, response) {
      var key = request.body.key;

      response.json({
        draw: parseInt(request.body.draw),
        recordsTotal: (storedDcpsStore[key] === undefined ? 0 : storedDcpsStore[key].length),
        recordsFiltered: (storedDcpsStore[key] === undefined ? 0 : storedDcpsStore[key].length),
        data: (storedDcpsStore[key] === undefined ? [] : storedDcpsStore[key].slice(parseInt(request.body.start), (parseInt(request.body.start) + parseInt(request.body.length))))
      });
    },

    removeStoredDcp: function(request, response) {
      var key = request.body.key;

      if(storedDcps[key] != undefined) {
        for(var i = 0, dcpsLength = storedDcps[key].length; i < dcpsLength; i++) {
          if(storedDcps[key][i].alias == request.body.alias) {
            storedDcps[key].splice(i, 1);
            break;
          }
        }
      }

      response.json(storedDcps[key]);
    },

    removeStoredDcpStore: function(request, response) {
      var key = request.body.key;

      if(storedDcpsStore[key] != undefined) {
        for(var i = 0, dcpsLength = storedDcpsStore[key].length; i < dcpsLength; i++) {
          if(storedDcpsStore[key][i].alias == request.body.alias) {
            storedDcpsStore[key].splice(i, 1);
            break;
          }
        }
      }

      response.json(storedDcpsStore[key]);
    },

    updateDcp: function(request, response) {
      var key = request.body.key;

      if(storedDcps[key] != undefined) {
        for(var i = 0, dcpsLength = storedDcps[key].length; i < dcpsLength; i++) {
          if(storedDcps[key][i].alias == request.body.oldAlias) {
            storedDcps[key][i] = request.body.dcp;
            break;
          }
        }
      }

      response.json(storedDcps[key]);
    },

    updateDcpStore: function(request, response) {
      var key = request.body.key;

      if(storedDcpsStore[key] != undefined) {
        for(var i = 0, dcpsLength = storedDcpsStore[key].length; i < dcpsLength; i++) {
          if(storedDcpsStore[key][i].alias == request.body.oldAlias) {
            storedDcpsStore[key][i] = request.body.dcp;
            break;
          }
        }
      }

      response.json(storedDcpsStore[key]);
    }
  };

};
