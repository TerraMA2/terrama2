"use strict"

var UriPattern = require('./Enums').Uri;
/**
 * Class responsible to check connection with data base
 * @class DbConnectionChecker
 * 
 * @author Francisco Vinhas [francisco.neto@funcate.org.br]
 * 
 * @property {object} json - Object with param to check connection.
 * @property {object} callback - Function to callback the result.
 * @property {object} memberPostgis - 'Postgis request' module.
 */

 var DbConnectionChecker = function(json, callback){
  // PostgisRequest class
  var memberPostgis = require("./PostgisRequest.js");

  var returnObject = {
    error: false,
    message: ""
  };

  var params = {};
  params[UriPattern.SCHEME] = "POSTGIS";
  params[UriPattern.HOST] = json.host;
  params[UriPattern.PORT] = json.port;
  params["database"] = json.database;
  params[UriPattern.USER] = json.user;
  params[UriPattern.PASSWORD] = json.password;

  try {
    var dbRequester = new memberPostgis(params);

    dbRequester.request().then(function() {
      returnObject.message = "Success";
      callback(returnObject);
    }).catch(function(err) {
      returnObject.error = true;
      returnObject.message = err.toString();

      callback(returnObject);
    })
  } catch (e) {
    returnObject.error = true;
    returnObject.message = "Invalid connection parameters";

    callback(returnObject);
  }

 };

module.exports = DbConnectionChecker;