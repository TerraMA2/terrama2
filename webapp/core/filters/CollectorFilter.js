'use strict';

// BaseClass
var Filter = require("./Filter");
// Enums
var DataSeriesType = require("./../Enums").DataSeriesType;
// DataModels
var DataSeries = require("./../data-model/DataSeries");


/**
 * Class responsible for filtering DataSeries from Collectors.
 */
var CollectorFilter = module.exports = function() {};

CollectorFilter.prototype = Object.create(Filter.prototype);
CollectorFilter.prototype.constructor = CollectorFilter;

/**
 * It applies a filter over data series to retrieve all the dynamic data series
 *  
 * @param {Array<Collector>} collectors - An array of TerraMA2 collectors
 * @param {Object} extra - An extra object to help filter
 * @param {Array<DataSeries>} extra.dataSeries - Data Series to filter
 * @return {Array<DataSeries>} It retrieves a filtered data series from collector
 */
CollectorFilter.prototype.match = function(collectors, extra) {
  var dataSeries = extra.dataSeries;
  // creating a copy
  var copyDataSeries = [];
  dataSeries.forEach(function(ds) {
    if (ds.data_series_semantics.data_series_type_name !== DataSeriesType.STATIC_DATA) {
      copyDataSeries.push(new DataSeries(ds));
    }
  });

  var output = [];

  copyDataSeries.forEach(function(element, index) {
    collectors.some(function(collector) {
      // collect
      if (collector.data_series_output === element.id) {
        return true;
      } else if (collector.data_series_input === element.id) {
        // removing input data series. TODO: improve this approach
        delete copyDataSeries[index];
        return true;
      }
      return false;
    });
  });

  // removing holes in array
  copyDataSeries.forEach(function(copyDs) {
    if (copyDs) { output.push(copyDs); }
  });

  return copyDataSeries;
};
