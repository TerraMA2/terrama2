var DataSeriesType = require('./Enums').DataSeriesType;
var FormField = require("./Enums").Form.Field;

/**
 * Generic DataSeriesSemantics type.
 * @constructor
 * @param {Object} params - It defines an object for semantics representation.
 */
var AbstractSemantics = module.exports = function(semanticsObject) {
  if (this.constructor === AbstractSemantics)
    throw new Error("AbstractSemantic cannot be instantiated");

  switch (semanticsObject.data_series_type_name) {
    case DataSeriesType.DCP:
      break;
    case DataSeriesType.OCCURRENCE:
      break;
    case DataSeriesType.GRID:
      break;
    default:
      throw new Error("Unknown data series type");
  }

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


/**
 * It defines the form structure of DataSeriesSemantics object. See more in @see Enums.FormField
 * @abstract
 * @return {Object}
 */
AbstractSemantics.schema = function() {
  return {
    active: {
      title: "Active",
      type: FormField.CHECKBOX,
      default: true
    }
  };
};

/**
 * It defines how DataSeriesSemantics object will display as GUI fields. See more in @see Enums.FormField
 * @abstract
 * @return {Object}
 */
AbstractSemantics.form = function() {
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
AbstractSemantics.databaseValues = function() {
  return {
    name: this.name,
    data_format_name: this.data_format_name,
    data_series_type_name: this.data_series_type_name
  }
};