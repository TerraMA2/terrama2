'use strict';

// Baseclass
var DataSet = require('./DataSet');
var logger = require("./../Logger");
// helpers
var isString = require("./../Utils").isString;

/**
 * It defines a DataSetDcp model representation.
 */
function DataSetDcp(params) {
  DataSet.call(this, params);

  this.positionWkt = params.positionWkt || null;

  this.setPosition(params.position);
}

// Javascript inherits model
DataSetDcp.prototype = Object.create(DataSet.prototype);
DataSetDcp.prototype.constructor = DataSetDcp;

/**
 * It sets geometry to DCP. The geometry may be a string (when loaded as ST_AsGeoJSON) or GeoJSON Javascript object
 * @param {string|Object} position - A geometry representation. It may be a GeoJSON string or GeoJSON object
 */
DataSetDcp.prototype.setPosition = function(position) {
  // Try parse if it is string
  if (isString(position)) {
    try {
      this.position = JSON.parse(position);
    } catch (e) {
      // ???
      logger.debug("Error during position parse. " + e.toString());
      this.position = position;
    }
  } else { // null / object
    this.position = position;
  }
};

/**
 * It retrieves a TerraMA² standard representation of DataSetOccurrence to send via TCP
 * @inherits DataSet:toObject
 */
DataSetDcp.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {position: this.positionWkt || this.position});
};

/**
 * It retrieves a TerraMA² real representation (database)
 * @inherits DataSet:rawObject
 */
DataSetDcp.prototype.rawObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {position: this.position});
};

module.exports = DataSetDcp;
