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
 * @property {object} memberPg - 'pg' module.
 * @property {object} memberUtils - 'Utils' model.
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
  // 'pg' module
  var memberPg = require('pg');
  // 'Utils' model
  var memberUtils = require('./Utils.js');

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
   * Returns the PostgreSQL psql connection string.
   * @param {integer} dataProviderId - Id to get the connection parameters in the DataProvider
   * @returns {Promise} Promise - Promise to be resolved
   *
   * @function getPsqlString
   * @memberof Exportation
   * @inner
   */
  this.getPsqlString = function(dataProviderId) {
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

        var connectionString = "psql -h " + uriObject.host + " -d " + database + " -U " + uriObject.user;
        var exportPassword = "export PGPASSWORD='" + uriObject.password + "';";

        return resolve({
          connectionString: connectionString,
          exportPassword: exportPassword
        });
      }).catch(function(err) {
        return reject(err);
      });
    });
  };

  /**
   * Verifies if a table exists.
   * @param {integer} tableName - Table name
   * @param {integer} dataProviderId - Id to get the connection parameters in the DataProvider
   * @returns {Promise} Promise - Promise to be resolved
   *
   * @function tableExists
   * @memberof Exportation
   * @inner
   */
  this.tableExists = function(tableName, dataProviderId) {
    var self = this;
    return new memberPromise(function(resolve, reject) {
      return memberDataManager.getDataProvider({ id: dataProviderId }).then(function(dataProvider) {
        var uriObject = memberUriBuilder.buildObject(dataProvider.uri, {
          HOST: 'host',
          PORT: 'port',
          USER: 'user',
          PASSWORD: 'password',
          PATHNAME: 'database'
        });

        uriObject.database = (uriObject.database.charAt(0) === '/' ? uriObject.database.substr(1) : uriObject.database);

        var client = new memberPg.Client(uriObject);

        client.on('error', function(err) {
          return reject(err.toString());
        });

        client.connect(function(err) {
          if(err)
            return reject(err.toString());

          client.query("SELECT table_name FROM information_schema.tables WHERE table_schema='public' AND table_name = $1;", [tableName], function(err, result) {
            client.end();

            if(err) return reject(err.toString());
            else return resolve(result);
          });
        });
      }).catch(function(err) {
        return reject(err.toString());
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
  * Returns the shp2pgsql application string.
  * @returns {string} shp2pgsql - shp2pgsql application
  *
  * @function shp2pgsql
  * @memberof Exportation
  * @inner
  */
  this.shp2pgsql = function() {
    var shp2pgsql = memberConfig.SHP2PGSQL;

    return shp2pgsql;
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

  /**
   * Returns a grid file path for a given data provider id and file mask.
   * @param {integer} dataProviderId - Data provider id
   * @param {string} mask - File mask
   * @param {string} date - Date to filter (undefined in case there's no date to filter)
   * @return {Promise} Promise - A 'bluebird' promise with a grid file path or error callback
   *
   * @function getGridFilePath
   * @memberof Exportation
   * @inner
   */
  this.getGridFilePath = function(dataProviderId, mask, date) {
    return new memberPromise(function(resolve, reject) {
      return memberDataManager.getDataProvider({ id: dataProviderId }).then(function(dataProvider) {
        var folder = dataProvider.uri.replace("file://", "");

        if(folder.substr(folder.length - 1) === "/")
          folder = folder.slice(0, -1);

        if(date !== undefined) {
          var dateObject = new Date(date.replace('Z', ''));

          var year = dateObject.getUTCFullYear().toString();
          var month = ('0' + (dateObject.getUTCMonth() + 1)).slice(-2);
          var day = ('0' + dateObject.getUTCDate()).slice(-2);
          var hours = ('0' + dateObject.getUTCHours()).slice(-2);
          var minutes = ('0' + dateObject.getUTCMinutes()).slice(-2);
          var seconds = ('0' + dateObject.getUTCSeconds()).slice(-2);

          mask = mask.split('%YYYY').join(year);
          mask = mask.split('%YY').join(year.substr(year.length - 2));
          mask = mask.split('%MM').join(month);
          mask = mask.split('%DD').join(day);
          mask = mask.split('%hh').join(hours);
          mask = mask.split('%mm').join(minutes);
          mask = mask.split('%ss').join(seconds);
        }

        return resolve(folder + "/" + mask);
      }).catch(function(err) {
        return reject(err);
      });
    });
  };

  /**
   * Returns a grid folder path for a given data provider id.
   * @param {integer} dataProviderId - Data provider id
   * @return {Promise} Promise - A 'bluebird' promise with a grid folder path or error callback
   *
   * @function getGridFolderPath
   * @memberof Exportation
   * @inner
   */
  this.getGridFolderPath = function(dataProviderId) {
    return new memberPromise(function(resolve, reject) {
      return memberDataManager.getDataProvider({ id: dataProviderId }).then(function(dataProvider) {
        var folder = dataProvider.uri.replace("file://", "");

        if(folder.substr(folder.length - 1) === "/")
          folder = folder.slice(0, -1);

        return resolve(folder);
      }).catch(function(err) {
        return reject(err);
      });
    });
  };

  /**
   * Creates a new folder in a given path.
   * @param {string} path - Path where the folder should be created
   * @return {object} object - Null in case of success, and error object otherwise
   *
   * @function createFolder
   * @memberof Exportation
   * @inner
   */
  this.createFolder = function(path) {
    try {
      memberFs.mkdirSync(path);
    } catch(e) {
      if(e.code != 'EEXIST')
        return e;
    }

    return null;
  };

  /**
   * Generates a new folder (in the temp directory) with a random name and the current date as suffix.
   * @return {Promise} Promise - A 'bluebird' promise with the name and path of the folder or error callback
   *
   * @function generateRandomFolder
   * @memberof Exportation
   * @inner
   */
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

        return resolve({
          filesFolder: filesFolder,
          folderPath: folderPath
        });
      });
    });
  };

  /**
   * Returns the proper file extension and ogr2ogr format string.
   * @param {string} format - Format
   * @return {object} object - Object containing the proper file extension and the ogr2ogr format string
   *
   * @function getFormatStrings
   * @memberof Exportation
   * @inner
   */
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

  /**
   * Returns the difference in days between the current date and a given date.
   * @param {string} dateString - Date (YYYY-MM-DD)
   * @returns {integer} difference - Difference between the dates
   *
   * @function getDateDifferenceInDays
   * @memberof Exportation
   * @inner
   */
  this.getDateDifferenceInDays = function(dateString) {
    var now = new Date();
    var date = new Date(dateString + " " + now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds());

    var utc1 = Date.UTC(now.getFullYear(), now.getMonth(), now.getDate());
    var utc2 = Date.UTC(date.getFullYear(), date.getMonth(), date.getDate());

    var difference = Math.floor((utc1 - utc2) / (1000 * 3600 * 24));

    return difference;
  };

  /**
   * Deletes the invalid folders (older than one day) from the tmp folder.
   *
   * @function deleteInvalidFolders
   * @memberof Exportation
   * @inner
   */
  this.deleteInvalidFolders = function() {
    var tmpDir = memberPath.join(__dirname, '../tmp');
    var dirs = memberFs.readdirSync(tmpDir).filter(file => memberFs.statSync(memberPath.join(tmpDir, file)).isDirectory());

    for(var i = 0, count = dirs.length; i < count; i++) {
      var dir = memberPath.join(__dirname, '../tmp/' + dirs[i]);
      var date = dirs[i].split('_--_');

      if(this.getDateDifferenceInDays(date[1]) > 1)
        memberUtils.deleteFolderRecursively(dir, function() {});
    }
  };

  /**
   * Copies a file from the source to the target, saving it with the given name.
   * @param {string} source - File source
   * @param {string} target - File target
   * @param {string} name - File new name
   *
   * @function copyFileSync
   * @memberof Exportation
   * @inner
   */
  this.copyFileSync = function(source, target, name) {
    var targetFile = target;

    if(memberFs.existsSync(target))
      if(memberFs.lstatSync(target).isDirectory())
        targetFile = memberPath.join(target, name + source.substr(source.length - 4));

    memberFs.writeFileSync(targetFile, memberFs.readFileSync(source));
  };

  /**
   * Copies the files of a Shapefile from a source to a target, naming them with the given name.
   * @param {string} source - Files source
   * @param {string} target - Files target
   * @param {string} name - Files new name
   *
   * @function copyShpFiles
   * @memberof Exportation
   * @inner
   */
  this.copyShpFiles = function(source, target, name) {
    var files = [];
    var self = this;

    if(memberFs.lstatSync(source).isDirectory()) {
      files = memberFs.readdirSync(source);

      files.forEach(function(file) {
        var currentSource = memberPath.join(source, file);

        if(!memberFs.lstatSync(currentSource).isDirectory() && file.substr(file.length - 4) !== ".zip")
          self.copyFileSync(currentSource, target, name);
      });
    }
  };
};

module.exports = Exportation;
