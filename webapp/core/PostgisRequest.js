var AbstractRequest = require("./AbstractRequest");
var pg = require('pg');
var Promise = require("bluebird");
var ConnectionError = require("./Exceptions").ConnectionError;
var Form = require('./Enums').Form;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");


function PostgisRequest(requestParameters) {
  AbstractRequest.apply(this, arguments);
}

PostgisRequest.prototype = Object.create(AbstractRequest.prototype);
PostgisRequest.prototype.constructor = PostgisRequest;

PostgisRequest.prototype.syntax = function() {
  var pattern = Object.assign({}, UriPattern);

  pattern.PATHNAME = "database";
  return pattern;
}

PostgisRequest.prototype.request = function() {
  var self = this;
  return new Promise(function(resolve, reject) {

    var client = new pg.Client(self.uri);

    client.connect(function(err) {
      if (err) {
        var errorMessage = "Error in postgis connection: ";
        switch (err.code) {
          case 'ENOTFOUND':
          case "ENETUNREACH": // host not found
            errorMessage += "Invalid host";
            break;
          case "ECONNREFUSED": // port error
            errorMessage += "Invalid port number";
            break;
          case "28P01": // username/password error
            errorMessage += "Username or password does not match";
            break;
          case "3D000": // Database does not exist
            errorMessage += "Database does not exist";
            break;
          default:
            break;
        }
        return reject(new ConnectionError(errorMessage));
      }
      client.end();
      resolve(true);
    });
  });
};

PostgisRequest.fields = function() {
  return Utils.makeCommonRequestFields("POSTGIS", 5432, "database", [UriPattern.HOST, UriPattern.PORT, UriPattern.USER, "database"],
      Utils.getCommonRequestFields().concat([{key: "database", type: Form.Field.TEXT, htmlClass: 'col-md-12'}]));
};

module.exports = PostgisRequest;