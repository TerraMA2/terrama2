var Application = require("./../core/Application");
var Promise = require('./../core/Promise');
var pg = require('pg');
var util = require('util');
var fs = require('fs');
var path = require('path');
var Sequelize = require('./Sequelize');
var Connection = require("sequelize").Connection;
var logger = require("./../core/Logger");

/**
 * @type {Connection}
 */
var sequelize = null;

/**
 * It is a TerraMA2 Database initialization module.
 * 
 * @class Database
 */
var Database = function(pathToConfig) {
  if (pathToConfig === undefined || !pathToConfig) {
    pathToConfig = path.join(__dirname, '/instances/default.json');
  }

  /**
   * It defines a current context key
   * 
   * @type {string}
   */
  this.currentContext = "default";
};

/**
 * It retrieves if orm is already initialized
 * 
 * @returns {boolean}
 */
Database.prototype.isInitialized = function() {
  return sequelize !== null;
};

/**
 * It retrieves a orm instance
 * 
 * @returns {Connection}
 */
Database.prototype.getORM = function() {
  return sequelize;
};

/**
 * It initializes database, creating database, schema and postgis support. Once prepared, it retrieves a ORM instance
 * 
 * @returns {Promise<Connection>}
 */
Database.prototype.init = function() {
  var self = this;

  return new Promise(function(resolve, reject) {
    if (self.isInitialized()) {
      return resolve(sequelize);
    }

    var currentContext = Application.getContextConfig();

    /**
     * Current database configuration context
     * 
     * @type {Object}
     */
    var contextConfig = currentContext.db;

    var schema = contextConfig.define.schema;
    var databaseName = contextConfig.database;
    var baseUri = util.format("postgres://%s:%s@%s", contextConfig.username, contextConfig.password, contextConfig.host);

    if (contextConfig.port) {
      baseUri += ":" + contextConfig.port;
    }

    var uri = baseUri;

    uri += "/postgres";

    pg.connect(uri, function(err, client, done) {
      if (err) {
        return reject(err);
      }

      var databaseQuery = util.format("CREATE DATABASE %s", databaseName);

      var _createSchema = function() {
        pg.connect(baseUri + "/" + databaseName, function(err, cli, dbDone) {
          if (!schema || schema === "") {
            dbDone();
            return resolve(sequelize);
          }
          var query = util.format("CREATE SCHEMA %s", schema);
          cli.query(query, function(err) {
            if (err && err.code !== "42P06") { // 42P06 -> Already exists
              return reject(err);
            }

            cli.query("CREATE EXTENSION postgis", function(err) {
              sequelize = new Sequelize(contextConfig);
              cli.end();
              resolve(sequelize);
            });
          });
        });
      };

      client.query(databaseQuery, function(err) {
        if (err) {
          if (err.code === "42P04") {
            logger.debug(err.toString());
          } else {
            logger.warn(err);
          }
        }
        client.end();
        _createSchema();
      });
    });
  });
};

/**
 * It closes database orm connection
 * 
 * @returns {Promise}
 */
Database.prototype.finalize = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (self.isInitialized()) {
      sequelize.close();
    }

    return resolve();
  });
};

module.exports = new Database();