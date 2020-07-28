const pg = require('pg');

/** Used when attempting to execute query but there is no client connected */
class NoConnectionError extends Error { }
/** Used when connection with postgres fails */
class ConnectionError extends Error { }
/** Used when attempting to execute query but got an error such syntax error, etc.  */
class QueryError extends Error { }

/**
 * An abstraction of PostgreSQL connection.
 *
 * You may use this class when intended to execute manual query in specific postgres instance
 *
 * **Important** Whenever connect to PostgreSQL instance, remember to call **disconnect** in order to close channel
 * with database
 *
 * @class Connection
 *
 * @todo Add support with URI Object instead only string
 *
 * @param {string} uri PostgreSQL URI connection
 *
 * @example
 * // Fake URI Example
 * const uri = 'postgresql://localhost/terrama2';
 * // Create instance connection
 * const connection = new Connection(uri);
 *
 * try {
 *   connection.connect();
 *   const result = await connection.execute(`SELECT now()`);
 *
 *   console.log(`Result is ${result.rows}`);
 * } catch {
 *   console.error(`Could not connect to database`);
 * } finally {
 *   if (connection.isConnected())
 *     connection.disconnect();
 * }
 */
class Connection {
  constructor(uri) {
    /**
     * PostgreSQL URI
     * @type {string}
     */
    this._uri = uri;
    /**
     * Internal pg client
     * @type {pg.Client}
     */
    this._client = null;
    /**
     * Internal pg done state
     * @type {any}
     */
    this._done = null;
    /**
     * Internal flag to handle active connection
     * @type {boolean}
     */
    this._connected = false;
  }

  /**
   * Tries to open connection with PostgreSQL.
   *
   * **if there is already an opened connection** just return
   *
   * @throws {ConnectionError} when could not connect to provided URI
   */
  connect() {
    return new Promise((resolve, reject) => {
      const { _uri } = this;

      if (this.isConnected())
        return resolve();

      pg.connect(_uri, (err, client, done) => {
        if (err)
          return reject(new ConnectionError(`Could not connect to Postgres due ${err.message}`));

        this._client = client;
        this._done = done;
        this._connected = true;

        return resolve();
      });
    });
  }

  /**
   * Disconnects the client channel
   *
   * **It is not supposed to throw exception**. You can call it even with closed connection.
   * But keep in mind that if there is a active connection and channel is busy, it can occurs
   * inconsistency and a generic error (from pg) may be thrown.
   *
   * @throws {ConnectionError} When could not disconnect from database
   */
  disconnect() {
    return new Promise((resolve, reject) => {
      if (!this.isConnected())
        return resolve();

      try {
        // this._client.end();
        this._done(this._client);
      } catch (err) {
        return reject(err);
      }

      this._client = null;
      this._done = null;
      this._connected = false;
      return resolve();
    });
  }

  /** Checks for active connection */
  isConnected() {
    return this._connected && this._client !== null && this._done !== null;
  }

  /** Retrieves internal pg client */
  getClient() {
    return this._client;
  }

  /**
   * Executes query statement into database
   *
   * @throws {NoConnectionError}
   * @throws {QueryError}
   * @param {string} sql Query statement
   * @returns {pg.QueryResult}
   */
  execute(sql) {
    return new Promise((resolve, reject) => {
      if (!this.isConnected())
        return reject(new NoConnectionError(`There is no connection with postgres. Make sure you are connected.`));

      const { _client } = this;

      _client.query(sql, (err, resultSet) => {
        if (err)
          return reject(new QueryError(`Error in query execution. ${err.message}`));

        return resolve(resultSet);
      })
    });
  }
}

const connectionModule = { Connection, ConnectionError, NoConnectionError, QueryError };

module.exports = connectionModule;