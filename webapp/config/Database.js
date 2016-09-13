var Promise = require('bluebird');
var pg = require('pg');
var util = require('util');
var fs = require('fs');
var path = require('path');
var Sequelize = require('./Sequelize');
var Connection = require("sequelize").Connection;

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
    pathToConfig = path.join(__dirname, 'config.terrama2');
  }

  /**
   * It stores a config.terrama2 settings
   * @type {Object} config
   */
  this.config = JSON.parse(fs.readFileSync(pathToConfig, 'utf-8'));

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
 * It retrieves a current context config
 * 
 * @returns {Object}
 */
Database.prototype.getContextConfig = function() {
  return this.config[this.currentContext];
};

/**
 * It sets current terrama2 context
 *
 * @throws {Error} When a contexts is not in config.terrama2 
 * @param {string} context
 * @returns {void}
 */
Database.prototype.setCurrentContext = function(context) {
  if (!context) {
    return;
  }
  // checking if there is a context in configuration file
  if (this.config && !this.config.hasOwnProperty(context)) {
    var msg = util.format("\"%s\" not found in configuration file. Please check \"webapp/config/config.terrama2\"", context);
    throw new Error(msg);
  }

  this.currentContext = context;
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

    var currentContext = self.getContextConfig();

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
            console.log(err.toString());
          } else {
            console.log(err);
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

    resolve();
  })
};

module.exports = new Database();