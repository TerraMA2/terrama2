'use strict';

/**
 * Facade Design Pattern - Analysis Registration
 *
 */
var Analysis = module.exports = { };

var DataManager = require("./../DataManager");
var PromiseClass = require("bluebird");
var AnalysisError = require("./../Exceptions").AnalysisError;
var Utils = require("./../Utils");

/**
 * @param {Object} analysisObject - An analysis object structure
 * @param {Object} scheduleObject - An schedule object structure
 * @param {Object} dataSeriesObject - An data series output structure
 * @return {Promise<Analysis>} a bluebird promise with Analysis instance value
 *
 * @example
 */
Analysis.save = function(analysisObject, scheduleObject, dataSeriesObject) {
  return new PromiseClass(function(resolve, reject) {
    DataManager.addDataSeries(dataSeriesObject, {
      data_series_id: analysisObject.data_series_id,
      type: analysisObject.type
    }).then(function(dataSeriesResult) {
      DataManager.addSchedule(scheduleObject).then(function(scheduleResult) {
        // adding analysis
        analysisObject.dataset_output = dataSeriesResult.dataSets[0].id;
        analysisObject.schedule_id = scheduleResult.id;

        DataManager.addAnalysis(analysisObject).then(function(analysisResult) {
          analysisResult.setSchedule(scheduleResult);
          analysisResult.setDataSeries(dataSeriesResult);

          resolve(analysisResult);
        }).catch(function(err) {
          // rollback analysis
          Utils.rollbackPromises([
            DataManager.removeDataSerie({id: dataSeriesResult.id}),
            DataManager.removeSchedule({id: scheduleResult.id})
          ], new AnalysisError("Could not save analysis " + err.toString()), reject);
        });
      }).catch(function(err) {
        // schedule
        Utils.rollbackPromises([
              DataManager.removeDataSerie({id: dataSeriesResult.id})],
            new AnalysisError("Could not save analysis schedule " + err.toString()), reject);
      });
    }).catch(function(err) { // data series
      console.log(err);
      reject(err);
    });
  });
};

Analysis.list = function(restriction) {
  return new PromiseClass(function(resolve, reject) {
    DataManager.listAnalyses(restriction).then(function(analysisList) {
      var output = [];
      analysisList.forEach(function(analysis) {
        output.push(analysis.rawObject());
      });
      resolve(output);
    }).catch(function(err) {
      reject(err);
    });
  });
};
