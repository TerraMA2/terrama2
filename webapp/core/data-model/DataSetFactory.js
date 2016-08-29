'use strict';

// dependencies
var DataSetError = require('./../Exceptions').DataSetError;
var DataSetDcp = require('./DataSetDcp');
var DataSetOccurrence = require('./DataSetOccurrence');
var DataSetGrid = require('./DataSetGrid');
var DataSetMonitored = require('./DataSetMonitored');
var DataSet = require("./DataSet");


/**
 * It builds a DataSet from given parameters
 * @param {Object} params - A javascript object with values to build
 * @return {DataSet} A data set object
 */
function build(params) {
  // checking if there a position key for DCP
  if (params.hasOwnProperty("position")) {
    return new DataSetDcp(params);
  }

  // checking for monitored object
  if (params.hasOwnProperty('time_column') && params.hasOwnProperty('geometry_column')) {
    return new DataSetMonitored(params);
  }

  if (params.hasOwnProperty('type') && (params.type == 'ANALYSIS-postgis' || params.type == 'ANALYSIS'))
    return new DataSet(params);
  // todo: validate it - occurrence}
  if (params.hasOwnProperty('id') && params.id > 0) {
    return new DataSetOccurrence(params);
  }

  throw new DataSetError("Invalid data set specified");
}

module.exports = {
  build: build
};
