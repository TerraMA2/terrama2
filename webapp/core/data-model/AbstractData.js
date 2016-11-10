'use strict';

/**
 * Generic class for DataModel.
 */
var AbstractData = function(params) {
  if (this.constructor === AbstractData) {
    throw new Error("Cannot instantiate abstract class");
  }
  this.class = params.class;
};

/**
 * It defines a TerraMA² standard representation of implementation to send via TCP.
 * Remember that JSON.stringify of model call by default toObject method parsed, which retrieves a string.
 * In nested objects, it may have inconsistents data.
 * @abstract
 * @return {Object} an object with class value. It must be implemented with model values 
 */
AbstractData.prototype.toObject = function() {
  return {
    "class": this.class
  };
};

/**
 * It defines a TerraMA² database values of model. It may be useful when filling out GUI forms or import/export a model
 * @abstract
 * @return {Object} an object with raw representation of model
 */
AbstractData.prototype.rawObject = function() {
  return {};
};

/**
 * It transforms the object to JSON.
 * @return {string} a json representation
 */
AbstractData.prototype.toJSON = function() {
  return JSON.stringify(this.toObject());
};

module.exports = AbstractData;