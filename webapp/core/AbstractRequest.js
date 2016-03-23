var UriBuilder = require("./UriBuilder");

var AbstractRequest = function(params) {
  if (this.constructor === AbstractRequest) {
    throw new Error("Cannot instantiate abstract class");
  }

  if (!params.path.startsWith("/"))
    params.path = "/" + params.path;

  this.params = params;
  this.uri = UriBuilder.buildUri(params);
};

AbstractRequest.prototype.request = function() {
  throw new Error("It must be implemented");
};

AbstractRequest.prototype.fields = function() {
  throw new Error("It must be implemented");
};

module.exports = AbstractRequest;