var DataSeriesType = require('./Enums').DataSeriesType;
var FormField = require("./Enums").Form.Field;
var Utils = require('./Utils');

/**
 * Generic DataSeriesSemantics type.
 * @constructor
 * @param {Object} params - It defines an object for semantics representation.
 */
var AbstractSemantics = module.exports = function(semanticsObject) {
  if (this.constructor === AbstractSemantics)
    throw new Error("AbstractSemantic cannot be instantiated");

  Utils.isValidDataSeriesType(semanticsObject.data_series_type_name);

  this.semantics = semanticsObject;
};

/**
 * It should retrieve an identifier to build in factory. The name is the same of data semantics name
 *
 * @abstract
 * @return {String}.
 */
AbstractSemantics.identifier = function() {
  throw new Error("AbstractSemantics identifier must be implemented");
};

/**
 * It retrieves a data semantics javascript object representation
 * {
 *   name: ...,
 *   data_series_format_name: ...,
 *   data_series_type_name: ...
 * }
 *
 * @abstract
 * @return {Object}.
 */
AbstractSemantics.prototype.get = function() {
  return this.semantics;
};

AbstractSemantics.prototype.metadata = function() {
  return this.semantics.metadata;
}

/**
 * It retrieves a list of DataProviders type that semantics works with.
 *
 * @abstract
 * @return {Array}.
 */
AbstractSemantics.prototype.demand = function() {
  return this.semantics.providers_type_list;
};


/**
 * It defines the form structure of DataSeriesSemantics object. See more in @see Enums.FormField
 * @abstract
 * @return {Object}
 */
AbstractSemantics.prototype.schema = function() {
  return {};
};

/**
 * It defines how DataSeriesSemantics object will display as GUI fields. See more in @see Enums.FormField
 * @abstract
 * @return {Object}
 */
AbstractSemantics.prototype.form = function() {
  return [
    {
      key: 'active',
      disableSuccessState: true,
      disableErrorState: true,
      htmlClass: 'col-md-1 terrama2-top-field'
    }
  ];
};

/**
 * It defines values to be stored in database.
 * @abstract
 * @return {Object}
 */
AbstractSemantics.prototype.databaseValues = function() {
  var semantics = this.get();
  return {
    name: semantics.name,
    code: semantics.code,
    data_format_name: semantics.data_format_name,
    data_series_type_name: semantics.data_series_type_name
  }
};
