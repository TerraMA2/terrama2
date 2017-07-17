"use strict";

/**
 * Exportation model, which contains exportation related database manipulations.
 * @class Exportation
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberPgFormat - 'pg-format' module.
 * @property {object} memberPromise - 'bluebird' module.
 * @property {object} memberDataManager - 'DataManager' module.
 * @property {object} memberUriBuilder - 'UriBuilder' module.
 * @property {object} memberConfig - Application configurations.
 */
var Exportation = function() {

  // 'pg-format' module
  var memberPgFormat = require('pg-format');
  // 'bluebird' module
  var memberPromise = require('bluebird');
  // 'DataManager' module
  var memberDataManager = require('./DataManager');
  // 'UriBuilder' module
  var memberUriBuilder = require('./UriBuilder');
  // Application configurations
  var memberConfig = require('./Application').getContextConfig();

   /**
    * Returns the PostgreSQL connection string.
    * @param {integer} dataProviderId - Id to get the connection parameters in the DataProvider
    * @returns {Promise} Promise - Promise to be resolved
    *
    * @function getPgConnectionString
    * @memberof Exportation
    * @inner
    */
   this.getPgConnectionString = function(dataProviderId) {
    return new memberPromise(function(resolve, reject) {
      return memberDataManager.getDataProvider({ id: dataProviderId }).then(function(dataProvider) {
        var uriObject = memberUriBuilder.buildObject(dataProvider.uri, {
          HOST: 'host',
          PORT: 'port',
          USER: 'user',
          PASSWORD: 'password',
          PATHNAME: 'database'
        });

        var database = (uriObject.database.charAt(0) === '/' ? uriObject.database.substr(1) : uriObject.database);

        var connectionString = "PG:host=" + uriObject.host + " port=" + uriObject.port + " user=" + uriObject.user + " password=" + uriObject.password + " dbname=" + database;

        return resolve(connectionString);
      }).catch(function(err) {
        return reject(err);
      });
    });
   };

   /**
    * Returns the ogr2ogr application string.
    * @returns {string} ogr2ogr - ogr2ogr application
    *
    * @function ogr2ogr
    * @memberof Exportation
    * @inner
    */
   this.ogr2ogr = function() {
     var ogr2ogr = memberConfig.OGR2OGR;

     return ogr2ogr;
   };

  /**
   * Returns the query accordingly with the received parameters.
   * @param {string} dateTimeFrom - Initial date / time
   * @param {string} dateTimeTo - Final date / time
   * @param {json} options - Filtering options
   * @returns {string} finalQuery - Query
   *
   * @function getQuery
   * @memberof Exportation
   * @inner
   */
  this.getQuery = function(dateTimeFrom, dateTimeTo, options) {
    // Creation of the query
    var query = "select * from " + options.Schema + "." + options.TableName + " where (" + options.DateTimeFieldName + " between %L and %L)",
        params = [dateTimeFrom, dateTimeTo];

    // Adds the query to the params array
    params.splice(0, 0, query);

    var finalQuery = memberPgFormat.apply(null, params);

    return finalQuery;
  };
};

module.exports = Exportation;
