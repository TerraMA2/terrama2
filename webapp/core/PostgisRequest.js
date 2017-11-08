var AbstractRequest = require("./AbstractRequest");
var pg = require('pg');
var Promise = require("bluebird");
var ConnectionError = require("./Exceptions").ConnectionError;
var GetObjectError = require("./Exceptions").GetObjectError;
var Form = require('./Enums').Form;
var UriPattern = require("./Enums").Uri;
var PostGISObjects = require("./Enums").PostGISObjects;
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

    client.on('error', function(err) { });

    client.connect(function(err) {
      if (err) {
        var errorMessage = "Error in PostGIS connection: ";
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
          case "28000": // username/password error
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

PostgisRequest.prototype.get = function (){
  var self = this;
  return new Promise(function(resolve, reject){
    var results = [];
    var client = new pg.Client(self.uri);

    client.on('error', function(err) { });

    client.connect(function(err){
      if (err) {
        var errorMessage = "Error in PostGIS connection: ";
        switch (err.code) {
          case 'ENOTFOUND':
          case "ENETUNREACH": // host not found
            errorMessage += "Invalid host";
            break;
          case "ECONNREFUSED": // port error
            errorMessage += "Invalid port number";
            break;
          case "28P01":
          case "28000": // username/password error
            errorMessage += "Username or password does not match";
            break; // username/password error
          case "3D000": // Database does not exist
            errorMessage += "Database does not exist";
            break;
          default:
            break;
        }
        return reject(new ConnectionError(errorMessage));
      }
      var query;
      if (self.params.objectToGet){
        switch (self.params.objectToGet){
          case PostGISObjects.DATABASE:
            query = "SELECT datname FROM pg_database WHERE datistemplate = false;";
            break;
          case PostGISObjects.TABLE:
            query = "SELECT table_name FROM information_schema.tables WHERE table_schema='public' AND table_type='BASE TABLE' AND table_name NOT LIKE 'spatial_ref_sys';";
            break;
          case PostGISObjects.COLUMN:
            query = "SELECT column_name FROM information_schema.columns WHERE table_schema='public' AND table_name='" + self.params.tableName + "';";
            break;
          case PostGISObjects.VALUES:
            query = "SELECT DISTINCT " + self.params.columnName + " FROM public." + self.params.tableName + " LIMIT 31;";
            break;
          default:
            return reject(new GetObjectError("Invalid object to query"));
        }
      } else {
        return reject(new GetObjectError("Invalid object to query"));
      }

      var queryResult = client.query(query);
      queryResult.on('row', (row) => {
        results.push(row);
      });
      queryResult.on('end', () => {
        client.end();
        if (self.params.objectToGet == PostGISObjects.VALUES){
          if (results.length == 31){
            results = [];
          } else {
            results = results.map(function(result){
              return result[self.params.columnName];
            });
          }
        }
        resolve(results);
      });
      queryResult.on('error', () =>{
        resolve();
      })
    });
  });
};

PostgisRequest.fields = function() {
  return Utils.makeCommonRequestFields("POSTGIS", 5432, "database", [UriPattern.HOST, UriPattern.PORT, UriPattern.USER, "database"],
      Utils.getCommonRequestFields().concat([{key: "database", type: Form.Field.TEXT, htmlClass: 'col-md-12'}]));
};

module.exports = PostgisRequest;
