var AbstractRequest = require('./AbstractRequest');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var NodeUtils = require('util');
var Requester = require('request');
var Form = require('./Enums').Form;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");

var HttpsRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

HttpsRequest.prototype = Object.create(AbstractRequest.prototype);
HttpsRequest.prototype.constructor = HttpsRequest;

HttpsRequest.prototype.request = function() {
  var self = this;
  return  new Promise(function(resolve, reject) {
    Requester(self.uri, function(err, resp, body) {
      if (err)
        reject(new Exceptions.ConnectionError("Error in https request"));
      else {
        if (resp.statusCode === 200)
          resolve();
        else
          reject(new Exceptions.ConnectionError(NodeUtils.format("Error in https request: (%d) - %s", resp.statusCode, resp.statusMessage)));
      }
    });
  });
};

HttpsRequest.fields = function() {
  return Utils.makeCommonRequestFields("HTTPS", 443, null, [UriPattern.HOST, UriPattern.PORT],
      Utils.getCommonRequestFields().concat([{key: UriPattern.PATHNAME, type: Form.Field.TEXT, htmlClass: 'col-md-12'}]));
};

module.exports = HttpsRequest;