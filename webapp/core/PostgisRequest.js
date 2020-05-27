var AbstractRequest = require("./AbstractRequest");
const { Connection } = require('./utility/connection');
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

/**
 * Retrieves string representation of Connection error
 * @param {Error} err
 */
const getErrorMessage = (err) => {
  switch (err.code) {
    case 'ENOTFOUND':
    case "ENETUNREACH": // host not found
      return "Invalid host";
    case "ECONNREFUSED": // port error
      return "Invalid port number";
    case "28P01": // username/password error
      return "Username or password does not match";
    case "28000": // username/password error
      return "Username or password does not match";
    case "3D000": // Database does not exist
      return "Database does not exist";
    default:
      return err.message;
  }
};

PostgisRequest.prototype = Object.create(AbstractRequest.prototype);
PostgisRequest.prototype.constructor = PostgisRequest;

PostgisRequest.prototype.syntax = function() {
  var pattern = Object.assign({}, UriPattern);

  pattern.PATHNAME = "database";
  return pattern;
}

PostgisRequest.prototype.request = function() {
  var self = this;
  return new Promise(async function(resolve, reject) {

    var client = new Connection(self.uri);

    try {
      await client.connect();
      await client.disconnect();

      return resolve(true);
    } catch (err) {
      var errorMessage = "Error in PostGIS connection: " + getErrorMessage(err);
      return reject(new ConnectionError(errorMessage));
    }
  });
};

PostgisRequest.prototype.get = function (){
  var self = this;
  return new Promise(async (resolve, reject) => {
    var client = new Connection(self.uri);

    try {
      await client.connect();

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
            query = "SELECT DISTINCT " + self.params.columnName + " FROM public." + self.params.tableName + " order by " + self.params.columnName + " LIMIT 31;";
            break;
          case PostGISObjects.ALLVALUES:
              query = "SELECT DISTINCT " + self.params.columnName + " FROM public." + self.params.tableName + " WHERE "+ self.params.tableName + " IS NOT NULL order by " + self.params.columnName + ";";
              break;
          case PostGISObjects.VIEWS:
              query = "SELECT table_name FROM information_schema.views WHERE table_name NOT IN ('geography_columns', 'geometry_columns', 'raster_columns', 'raster_overviews') AND table_schema='public';";
              break;
          default:
            return reject(new GetObjectError("Invalid object to query"));
        }
      } else {
        return reject(new GetObjectError("Invalid object to query"));
      }

      let queryResult = await client.execute(query);
      let results = queryResult.rows;

      if (self.params.objectToGet === PostGISObjects.VALUES) {
        if (results.length === 31){
          results = [];
        } else {
          results = results.map(result => result[self.params.columnName]);
        }
      }else if (self.params.objectToGet === PostGISObjects.ALLVALUES){
        var listValues = results.map(result => result[self.params.columnName]);
        return resolve(listValues);
      }

      return resolve(results);
    } catch (err) {
      var errorMessage = "Error in PostGIS connection: " + getErrorMessage(err);

      return reject(new ConnectionError(errorMessage));
    } finally {
      await client.disconnect();
    }
  });
};

PostgisRequest.fields = function() {
  return Utils.makeCommonRequestFields("POSTGIS", 5432, "database", [UriPattern.HOST, UriPattern.PORT, UriPattern.USER, "database"],
      Utils.getCommonRequestFields().concat([{key: "database", type: Form.Field.TEXT, htmlClass: 'col-md-12'}]));
};

module.exports = PostgisRequest;
