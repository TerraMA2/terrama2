var AbstractRequest = require('./AbstractRequest');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var NodeUtils = require('util');
var Requester = require('request');
var UriBuilder = require("./UriBuilder");

var HttpRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

HttpRequest.prototype = Object.create(AbstractRequest.prototype, {
  'constructor': HttpRequest
});

HttpRequest.prototype.request = function() {
  var self = this;
  return  new Promise(function(resolve, reject) {

    var uri = UriBuilder.buildUri(self.params);

    Requester(uri, function(err, resp, body) {
      if (err)
        reject(new Exceptions.ConnectionError("Error in http request"));
      else {
        if (resp.statusCode === 200)
          resolve();
        else
          reject(new Exceptions.ConnectionError(NodeUtils.format("Error in http request: (%d) - %s", resp.statusCode, resp.statusMessage)));
      }
    });
  });
};


module.exports = HttpRequest;