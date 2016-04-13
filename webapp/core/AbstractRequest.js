var UriBuilder = require("./UriBuilder");
var UriPattern = require("./Enums").Uri;


/**
 * Generic Request type.
 * @constructor
 * @param {Object} params - It defines an object that will be converted to an uri.
 */
var AbstractRequest = function(params) {
  if (this.constructor === AbstractRequest) {
    throw new Error("Cannot instantiate abstract class");
  }

  if (params instanceof Object) {
    var splitHost = (params[this.syntax().HOST] || "").split("://");
    if (splitHost.length > 1)
      params[this.syntax().HOST] = splitHost[1];
    
    this.params = params;
    this.uri = UriBuilder.buildUri(params, this.syntax());
  }
  else if (typeof params === "string") {
    this.uri = params;
    this.params = UriBuilder.buildObject(params, this.syntax());
  }
};

/**
 * It applies request operation.
 * @abstract
 * @return {Promise}
 */
AbstractRequest.prototype.request = function() {
  throw new Error("It must be implemented");
};

/**
 * It retrieves uri object of request mapped in class syntax.
 * @abstract
 * @return {Object}
 */
AbstractRequest.prototype.uriObject = function() {
  return this.params;
};

/**
* It maps how to build an URI. Enums:Uri
*/
AbstractRequest.prototype.syntax = function() {
  // throw new Error("Abstract member 'syntax' must be implemented.");
  return UriPattern;
};

/**
 * It defines the structure of request object and how to display it as GUI fields. See more in @see Enums.FormField
 * @abstract
 * @return {Promise}
 */
AbstractRequest.fields = function() {
  throw new Error("It must be implemented");
};

module.exports = AbstractRequest;