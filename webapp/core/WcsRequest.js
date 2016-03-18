var AbstractRequest = require('./AbstractRequest');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");

var WcsRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

WcsRequest.prototype = Object.create(AbstractRequest.prototype, {
  'constructor': WcsRequest
});

WcsRequest.prototype.request = function() {
  var self = this;
  return  new Promise(function(resolve, reject) {
    reject(new Exceptions.ConnectionError("WCS is not supported yet"));
  });
};


module.exports = WcsRequest;