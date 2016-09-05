'use strict';

// dependency
var DataSet = require('./DataSet');

/**
 * A TerraMA² DataSetGrid representation
 */
function DataSetGrid(params) {
  DataSet.call(this, params);
}
// javascript inherits model
DataSetGrid.prototype = Object.create(DataSet.prototype);
DataSetGrid.prototype.constructor = DataSetGrid;

DataSetGrid.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {});
};

module.exports = DataSetGrid;
