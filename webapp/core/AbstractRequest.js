var AbstractRequest = function(params) {
  if (this.constructor === AbstractRequest) {
    throw new Error("Cannot instantiate abstract class");
  }

  this.params = params;
};

AbstractRequest.prototype.request = function() {
  throw new Error("It must be implemented");
};

module.exports = AbstractRequest;