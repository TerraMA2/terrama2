const pg = require('pg');
const path = require('path');
const Umzug = require('umzug')
const Application = require("./../Application");
const logger = require('./../Logger');
const Promise = require('./../Promise');
const Sequelize = require('./Sequelize');

/**
 * @type {Connection}
 */
var sequelize = null;

/**
 * Execute query on pg client.
 *
 * This method is used only in TerraMA² startup to create database, schema and extension.
 * In other words, it already handles error code for "Exists" to skip and resolve promise
 *
 * @param {string} query Query statement
 * @param {pg.Client} client PostgreSQL Client
 */
function executeQuery(query, client) {
  return new Promise((resolve, reject) => {
    if (!client)
      return reject(new Error("Database client is closed."));

    client.query(query, (err, result) => {
      if (!err)
        return resolve(result)

      switch(err.code) {
        // When already exists, return true
        case '42P06':
          return resolve(result);
        default:
          return reject(err);
      }
    });
  });
}

/**
 * Utility function to check if query returns result set.
 *
 * @param {string} query SQL Query statement
 * @param {pg.Client} client PostgreSQL client
 * @returns {Promise<boolean>}
 */
function exists(query, client) {
  return executeQuery(query, client)
    .then(found => (
      Promise.resolve(found.rows.length !== 0)
    ));
}

/**
 * Tries to create Schema and Extension for TerraMA²
 *
 * @param {pg.Client} client PG client
 * @param {string} schema Schema name to create
 * @param {any} config Current Application config
 * @returns Promise<Sequelize>
 */
async function createStructures(client, schema, config) {
  try {
    // First of all, we need check if schema already exists
    const foundSchema = await exists(`SELECT schema_name FROM information_schema.schemata WHERE schema_name = '${schema}'`, client);

    // When not found, tries to create. Remember that user must have right permission
    if (!foundSchema) {
      await executeQuery(`CREATE SCHEMA ${schema}`, client);
    }

    // Same for extension that requires superuser rights
    const foundExtension = await exists(`SELECT extname FROM pg_extension WHERE extname = 'postgis'`, client);

    if (!foundExtension) {
      await executeQuery(`CREATE EXTENSION postgis`, client);
    }

    // Set sequelize db config
    sequelize = new Sequelize(config);

    client.end();
    return sequelize;
  } catch (err) {
    client.end();
    throw err;
  }
}

async function prepareMigrations() {
  const migrator = new Umzug({
    storage: 'sequelize',
    storageOptions: { sequelize },
    migrations: {
      params: [
        sequelize.getQueryInterface(),
        require('sequelize')
      ],
      path: path.resolve(__dirname, '../../migrations')
    }
  })

  try {
    await migrator.up();
    logger.info(`Migrations loaded.`)

    await prepareInitialValues();

    return sequelize;
  } catch (err) {
    console.log(err);
    throw err;
  }
}

async function prepareInitialValues() {
  const migrator = new Umzug({
    storage: 'sequelize',
    storageOptions: { sequelize },
    migrations: {
      params: [
        sequelize.getQueryInterface(),
        require('sequelize')
      ],
      path: path.resolve(__dirname, '../../seeders')
    }
  })

  try {
    await migrator.up();
    logger.info(`Done.`)

    return sequelize;
  } catch (err) {
    console.log(err);
    throw err;
  }
}

/**
 * It is a TerraMA2 Database initialization module.
 *
 * Tries to prepare an environment for TerraMA², creating the databases and configure extensions.
 *
 * This module is used by DataManager in order to load tables and defaults rows in database.
 *
 * Whenever initialize database, remember to call .finalize in order to close connections properly
 *
 * @example
 *
 * const database = require('./config/Database');
 *
 * database.init()
 *   .then(orm => {
 *     // use orm directly
 *
 *     // close database connections (promise)
 *     database.finalize()
 *   })
 *   .catch(err => console.error(err))
 *
 * @class Database
 */
class Database {
  constructor() {
    /**
     * It defines a current context key
     *
     * @type {string}
     */
    this.currentContext = "default";
  }

  /**
   * It retrieves if orm is already initialized
   *
   * @returns {boolean}
   */
  isInitialized() {
    return sequelize !== null;
  }

  /**
   * It retrieves a orm instance
   *
   * @returns {Connection}
   */
  getORM() {
    return sequelize;
  };

  /**
   * Tries to connect to PostgreSQL and retrieves a client executor
   *
   * @param {string} uri URI connection
   * @returns Promise<Connection>
   */
  connect(uri) {
    return new Promise((resolve, reject) => (
      pg.connect(uri, (err, client, done) => {
        this.client = client;
        this.done = done;

        if (err)
          reject(err)
        else
          resolve(client)
      })
    ));
  }

  /**
   * It closes database orm connection
   *
   * @returns {Promise}
   */
  finalize() {
    return new Promise(resolve => {
      if (this.isInitialized()) {
        sequelize.close();
      }

      return resolve();
    });
  };

  /**
   * It initializes database, creating database, schema and postgis support. Once prepared, it retrieves a ORM instance
   *
   * @returns {Promise<Connection>}
   */
  init() {
    return new Promise((resolve, reject) => {
      if (this.isInitialized())
        return resolve(sequelize);

      var currentContext = Application.getContextConfig();

      /**
       * Current database configuration context
       *
       * @type {Object}
       */
      var contextConfig = currentContext.db;

      var schema = contextConfig.define.schema;
      var databaseName = contextConfig.database;
      var uri = `postgres://${contextConfig.username}:${contextConfig.password}@${contextConfig.host}`;

      if (contextConfig.port) {
        uri += ":" + contextConfig.port;
      }

      /**
       * Wrap connection method in order to create data structures
       *
       * @param {string} uri String URI to the database
       * @returns {Promise<Sequelize>}
       */
      const wrapConnect = () => {
        return this.connect(`${uri}/${databaseName}`)
          // On success, just proceed and create schema
          .then(client => createStructures(client, schema, contextConfig))
          .then(() => resolve(prepareMigrations()))
      };

      // Tries to connect real database and proceed with data structures creation
      return wrapConnect()
        // On error, tries to connect to PostgreSQL to create database manually
        .catch(err => {
          // When code is related to pg_hba.conf, stop promise chain.
          if (err.code === '28000') {
            return reject(err);
          }

          logger.warn(`Could not connect to database ${databaseName}. Is it exists?\nTrying connect to database "postgres" in order to create database ${databaseName}...`)

          return this.connect(uri + "/postgres")
            .then(() => executeQuery(`CREATE DATABASE ${databaseName}`, this.client))
            .then(() => {
              // Close channel with Postgres
              this.client.end();
              // Reconnect to the database provided
              return wrapConnect();
            })
            .catch(error => reject(error));
        });
    });
  }
}

module.exports = new Database();