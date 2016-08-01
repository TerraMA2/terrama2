var Promise = require('bluebird');
var pg = require('pg');
var util = require('util');
var fs = require('fs');
var path = require('path');
var Sequelize = require('./Sequelize');


var sequelize = null;

/**
 * It is a TerraMA2 Database initialization module.
 */
var Database = function(pathToConfig) {
  if (pathToConfig === undefined || !pathToConfig)
    pathToConfig = path.join(__dirname, 'config.terrama2');

  // reading terrama2 database file configuration
  this.config = JSON.parse(fs.readFileSync(pathToConfig, 'utf-8'));
};

Database.prototype.isInitialized = function() {
  return sequelize !== null;
};

Database.prototype.getORM = function() {
  return sequelize;
};

Database.prototype.init = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (self.isInitialized())
      return resolve(sequelize);

    var schema = self.config.define.schema;
    var databaseName = self.config.database;
    var baseUri = util.format("postgres://%s:%s@%s", self.config.username, self.config.password, self.config.host);

    if (self.config.port)
      baseUri += ":" + self.config.port;

    var uri = baseUri;

    uri += "/postgres";

    pg.connect(uri, function(err, client, done) {
      if (err) {
        return reject(err);
      }

      var databaseQuery = util.format("CREATE DATABASE %s", databaseName);

      var _createSchema = function() {
        pg.connect(baseUri + "/" + databaseName, function(err, cli, dbDone) {
          var query = util.format("CREATE SCHEMA %s", schema);
          cli.query(query, function(err) {
            if (err && err.code !== "42P06") { // 42P06 -> Already exists
              return reject(err);
            }

            cli.query("CREATE EXTENSION postgis", function(err) {
              sequelize = new Sequelize(self.config);
              cli.end();
              resolve(sequelize);
            });
          })
        });
      };

      client.query(databaseQuery, function(err) {
        if (err) {
          if (err.code === "42P04")
            console.log(err.toString());
          else
            console.log(err);
        }
        client.end();
        _createSchema();
      })
    });
  });
};

Database.prototype.finalize = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (self.isInitialized())
      sequelize.close();

    resolve();
  })
};

module.exports = new Database();