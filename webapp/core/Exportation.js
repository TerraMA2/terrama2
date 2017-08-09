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
 * @property {object} memberFs - 'fs' module.
 * @property {object} memberPath - 'path' module.
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
  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');

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
   * @param {json} options - Filtering options
   * @returns {string} finalQuery - Query
   *
   * @function getQuery
   * @memberof Exportation
   * @inner
   */
  this.getQuery = function(options) {
    // Creation of the query
    var query = "select * from " + options.Schema + "." + options.TableName;
    
    if(options.dateTimeField !== undefined && options.dateTimeFrom !== undefined && options.dateTimeTo !== undefined) {
      query += " where (" + options.dateTimeField + " between %L and %L)";
      var params = [options.dateTimeFrom, options.dateTimeTo];

      // Adds the query to the params array
      params.splice(0, 0, query);

      var finalQuery = memberPgFormat.apply(null, params);
    } else {
      var finalQuery = query;
    }

    return finalQuery;
  };

  this.getGridFilePath = function(dataProviderId, mask) {
    return new memberPromise(function(resolve, reject) {
      return memberDataManager.getDataProvider({ id: dataProviderId }).then(function(dataProvider) {
        var folder = dataProvider.uri.replace("file://", "");

        if(folder.substr(folder.length - 1) === "/")
          folder = folder.slice(0, -1);

        return resolve(folder + "/" + mask);
      }).catch(function(err) {
        return reject(err);
      });
    });
  };

  this.createFolder = function(path) {
    try {
      memberFs.mkdirSync(path);
    } catch(e) {
      if(e.code != 'EEXIST')
        return e;
    }

    return null;
  };

  this.generateRandomFolder = function() {
    var self = this;

    return new memberPromise(function(resolve, reject) {
      require('crypto').randomBytes(24, function(err, buffer) {
        if(err)
          return reject(err);

        var today = new Date();

        var dd = today.getDate();
        var mm = today.getMonth() + 1;
        var yyyy = today.getFullYear();

        if(dd < 10) dd = '0' + dd;
        if(mm < 10) mm = '0' + mm;

        var todayString = yyyy + '-' + mm + '-' + dd;
        var filesFolder = buffer.toString('hex') + '_--_' + todayString;
        var folderPath = memberPath.join(__dirname, '../tmp/' + filesFolder);

        var folderResult = self.createFolder(folderPath);

        if(folderResult)
          return reject(folderResult);

        return resolve(filesFolder, folderPath);
      });
    });
  };

  this.getFormatStrings = function(format) {
    switch(format) {
      case 'csv':
        var fileExtention = '.csv';
        var ogr2ogrFormat = 'CSV';
        break;
      case 'shapefile':
        var fileExtention = '.shp';
        var ogr2ogrFormat = 'ESRI Shapefile';
        break;
      case 'kml':
        var fileExtention = '.kml';
        var ogr2ogrFormat = 'KML';
        break;
      default:
        var fileExtention = '.json';
        var ogr2ogrFormat = 'GeoJSON';
    }

    return {
      fileExtention: fileExtention,
      ogr2ogrFormat: ogr2ogrFormat
    };
  };
};

module.exports = Exportation;
