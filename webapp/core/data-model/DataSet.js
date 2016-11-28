'use strict';

// base class
var AbstractClass = require("./AbstractData");

/**
 * A generic representation of DataSet
 * 
 * @property id
 * @property data_series_id
 * @property active
 * @property format
 */
function DataSet(params) {
  AbstractClass.call(this, {'class': 'DataSet'});
  
  this.id = params.id;
  this.data_series_id = params.data_series_id;
  this.active = params.active || false;

  if (params.DataSetFormats)
    this.setFormat(params.DataSetFormats);
  else
    this.format = params.format || {};
}

// javascript inherits model
DataSet.prototype = Object.create(AbstractClass.prototype);
DataSet.prototype.constructor = DataSet;

/**
 * It prepares a object from format specified. 
 * @param {Array<Object>|Object} formats - format values
 */
DataSet.prototype.setFormat = function(formats) {
  var formatOutput = {};
  if (formats instanceof Array) {
    formats.forEach(function(format) {
      formatOutput[format.key] = format.value;
    });
  } else {
    formatOutput = formats;
  }
  
  this.format = formatOutput;
};

/**
 * It retrieves a TerraMA² standard representation of DataSet to send via TCP
 * @inherits AbstractData:toObject
 */
DataSet.prototype.toObject = function() {
  return Object.assign(AbstractClass.prototype.toObject.call(this), {
    id: this.id,
    data_series_id: this.data_series_id,
    active: this.active,
    format: this.format
  });
};

module.exports = DataSet;