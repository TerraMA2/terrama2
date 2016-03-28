var AbstractRequest = require("./AbstractRequest");
var pg = require('pg');
var Promise = require("bluebird");
var ConnectionError = require("./Exceptions").ConnectionError;
var FormField = require('./Enums').FormField;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");


function PostgisRequest(requestParameters) {
  AbstractRequest.apply(this, arguments);
}

PostgisRequest.prototype = Object.create(AbstractRequest, {
  constructor: PostgisRequest
});

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
  var properties = {};
  properties[UriPattern.HOST] = {
    title: "Address",
    type: FormField.TEXT
  };

  properties[UriPattern.PORT] = {
    title: "Port",
    type: FormField.NUMBER,
    default: 5432
  };

  properties[UriPattern.USER] = {
    title: "Username",
    type: FormField.TEXT
  };

  properties[UriPattern.PASSWORD] = {
    title: "Password",
    type: FormField.TEXT
  };

  properties["database"] = {
    title: "DataBase name",
    type: FormField.TEXT
  };

  return Utils.makeCommonRequestFields("POSTGIS", 5432, "database", [UriPattern.HOST, UriPattern.PORT, UriPattern.USER, "database"], [
      UriPattern.HOST,
      UriPattern.PORT,
      UriPattern.USER,
      {
        key: UriPattern.PASSWORD,
        type: FormField.PASSWORD
      },
      "database"
    ]);
}

module.exports = PostgisRequest;