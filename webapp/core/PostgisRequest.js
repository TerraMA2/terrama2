var AbstractRequest = require("./AbstractRequest");
var pg = require('pg');
var UriBuilder = require("./UriBuilder");
var Promise = require("bluebird");
var ConnectionError = require("./Exceptions").ConnectionError;


function PostgisRequest(requestParameters) {
  AbstractRequest.apply(this, arguments);
}

PostgisRequest.prototype = Object.create(AbstractRequest, {
  constructor: PostgisRequest
});

PostgisRequest.prototype.request = function() {
  var self = this;
  return new Promise(function(resolve, reject) {

    var uri = UriBuilder.buildUri(self.params);

    var client = new pg.Client(uri);

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


module.exports = PostgisRequest;