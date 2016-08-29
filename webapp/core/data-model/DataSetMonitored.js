'use strict';

// base class
var DataSet = require('./DataSet');

/**
 * It defines a DataSetMonitored model representation.
 */
function DataSetMonitored(params) {
  DataSet.call(this, params);
  this.geometry_column = params.geometry_column;
  this.time_column = params.time_column;
  this.id_column = params.id_column;
  this.srid = params.srid;
}

// javascript inherits model
DataSetMonitored.prototype = Object.create(DataSet.prototype);
DataSetMonitored.prototype.constructor = DataSetMonitored;

/**
 * It retrieves a TerraMA² real representation (database)
 * @inherits DataSet:rawObject
 */
DataSetMonitored.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {
    geometry_column: this.geometry_column,
    time_column: this.time_column,
    srid: this.srid,
    id_column: this.id_column
  });
};

module.exports = DataSetMonitored;