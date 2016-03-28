var UriBuilder = require("./UriBuilder");
var UriPattern = require("./Enums").Uri;

var AbstractRequest = function(params) {
  if (this.constructor === AbstractRequest) {
    throw new Error("Cannot instantiate abstract class");
  }

  this.params = params;
  this.uri = UriBuilder.buildUri(params, this.syntax());
  console.log(this.uri);
};

AbstractRequest.prototype.request = function() {
  throw new Error("It must be implemented");
};

/**
* It maps how to build an URI. Enums:Uri
*/
AbstractRequest.prototype.syntax = function() {
  // throw new Error("Abstract member 'syntax' must be implemented.");
  return UriPattern;
}

AbstractRequest.fields = function() {
  /**
   * It should retrieve a object with fields for request. i.e
   *
   * {
   *   name: "FILE",
   *   properties: {
   *     path: {
   *       title: .. //! It represents field name in form,
   *       type: .. //! Define input type. "number", "text"...
   *     },
   *     user: ...
   *     password: ...
   *
   *   }
   * }
   *
   */
  throw new Error("It must be implemented");
};

module.exports = AbstractRequest;