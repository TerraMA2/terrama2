/**
 * This file contains TerraMA2 common exceptions
 * @class Errors
*/
var util = require('util');
var errors = module.exports = {};


/**
 * The Base Error all TerraMA2 Errors inherit from.
 * @class BaseError
*/
errors.BaseError = function() {
  var tmp = Error.apply(this, arguments);
  tmp.name = this.name = 'BaseError';

  this.message = tmp.message;

  if (Error.captureStackTrace)
    Error.captureStackTrace(this, this.constructor);
};
util.inherits(errors.BaseError, Error);


/**
 * Thrown when DataManager class has failed during either database connection or database operation
 *
 * @param {string} message Error message
 *
 * @extends BaseError
*/
errors.DataManagerError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'DataManagerError';
  this.message = 'DataManager Error';

  // Use provided error message if available...
  if (message) {
    this.message = message;

    // ... otherwise create a concatenated message out of existing errors.
  } else if (this.errors.length > 0 && this.errors[0].message) {
    this.message = this.errors.map(function(err) {
      return err.type + ': ' + err.message;
    }).join(',\n');
  }
};
util.inherits(errors.DataManagerError, errors.BaseError);


/**
 * Thrown when DataProvider object has inconsistent data.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.DataProviderError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'DataProviderError';
};
util.inherits(errors.DataProviderError, errors.BaseError);


/**
 * Thrown when DataSeries object has inconsistent data.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.DataSeriesError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'DataSeriesError';
};
util.inherits(errors.DataSeriesError, errors.BaseError);


/**
 * Thrown when DataSet object has inconsistent data.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.DataSetError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'DataSetError';
};
util.inherits(errors.DataSetError, errors.BaseError);


/**
 * Thrown when Project object has inconsistent data.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.ProjectError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'ProjectError';
};
util.inherits(errors.ProjectError, errors.BaseError);

/**
 * Thrown when User object has inconsistent data.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.UserError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'UserError';
};
util.inherits(errors.UserError, errors.BaseError);


/**
 * Thrown when DataSeriesSemanticsError object has inconsistent data.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.DataSeriesSemanticsError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'DataSeriesSemanticsError';
};
util.inherits(errors.DataSeriesSemanticsError, errors.BaseError);


/**
 * Thrown when DataFormatError object has inconsistent data.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.DataFormatError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'DataFormatError';
};
util.inherits(errors.DataFormatError, errors.BaseError);


/**
 * Thrown when check connection has failed.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.ConnectionError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'ConnectionError';
};
util.inherits(errors.ConnectionError, errors.BaseError);


/**
 * Thrown when URI ill-formed.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.UriError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'UriError';
};
util.inherits(errors.UriError, errors.BaseError);


/**
 * Thrown when could not load Plugin.
 *
 * @param {string} message Error message
 *
 * @extends BaseError
 */
errors.PluginError = function(message) {
  errors.BaseError.apply(this, arguments);
  this.name = 'PluginError';
};
util.inherits(errors.PluginError, errors.BaseError);
