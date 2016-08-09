"use strict";

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
 * @param {Array<Collector>} collectors - An array of TerraMA2 collectors
 * @param {Object} extra - An extra object with following syntax: {"dataSeries": Array<DataSeries>"}
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
    console.log(element.name);
    collectors.some(function(collector) {
      // collect
      if (collector.output_data_series === element.id) {
        return true;
      } else if (collector.input_data_series === element.id) {
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