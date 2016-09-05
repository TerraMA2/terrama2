'use strict';

// base class
var DataSet = require('./DataSet');

/**
 * It represents a data set occurrence structure.
 * @constructor
 * @params {Object} params - A javascript object to build DataSet
 * 
 * @inherits DataSet
 */
function DataSetOccurrence(params) {
  DataSet.call(this, params);
}

// Javascript inherits model
DataSetOccurrence.prototype = Object.create(DataSet.prototype);
DataSetOccurrence.prototype.constructor = DataSetOccurrence;


/**
 * It retrieves a TerraMA² standard representation of DataSetOccurrence to send via TCP
 * 
 * @inherits DataSet:toObject
 */
DataSetOccurrence.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {});
};

/**
 * It retrieves a TerraMA² real representation (database)
 * @inherits DataSet:rawObject
 */
DataSetOccurrence.prototype.rawObject = function() {
  return this.toObject();
}

module.exports = DataSetOccurrence;