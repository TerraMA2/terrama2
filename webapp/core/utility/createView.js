const { Connection } = require('./connection');
const logger = require('./../Logger');

/** Used when no empty result set found */
class EmptyViewError extends Error { };

/**
 * Internal function to generate SQL View
 *
 * @param {string} tableName Table name to search for
 * @param {string[]?} attributes List of attributes to retrieve
 * @param {string?} whereCondition Query Restriction
 */
const prepareSelect = (tableName, attributes = [], whereCondition = '') => {
  if (!attributes || attributes.length === 0)
    attributes = ['*'];

  if (!whereCondition)
    whereCondition = '1=1';

  return `SELECT ${attributes.join(',')} FROM ${tableName} WHERE ${whereCondition}`;
};

/**
 * Internal function to retrieve PostgreSQL client
 *
 * @param {string} uri
 */
const executeQuery = async (uri, query) => {
  const connection = new Connection(uri);

  try {
    await connection.connect();
    const result = await connection.execute(query);
    return result;
  } catch (err) {
    logger.debug(`Error in creating view: ${err.message}`);
    throw err;
  } finally {
    await connection.disconnect();
  }
};

/**
 * Validates view generation, checking both SQL syntax and result set size
 *
 * @throws {EmptyViewError} when result set is empty
 *
 * @param {string} uri Database URI
 * @param {string} tableName Table name to search for
 * @param {string[]?} attributes List of attributes to retrieve
 * @param {string?} whereCondition Query Restriction
 */
async function validateView(uri, tableName, attributes, whereCondition) {
  const sql = prepareSelect(tableName, attributes, whereCondition);

  const resultSet = await executeQuery(uri, sql);

  if (resultSet.rowCount === 0)
    throw new EmptyViewError(`The view result is empty. Make sure the query builder has been set properly`);

  return resultSet;
}

/**
 * Create view into specific Data Provider PostGIS instance
 *
 * @param {string} uri Database URI
 * @param {string} viewName View name to create
 * @param {string} tableName Table name to search for
 * @param {string[]?} attributes List of attributes to retrieve
 * @param {string?} whereCondition Query Restriction
 */
async function createView(uri, viewName, tableName, attributes = [], whereCondition = '1=1') {
  const sql = `
    DROP VIEW IF EXISTS ${viewName};
    CREATE OR REPLACE VIEW ${viewName} AS ${prepareSelect(tableName, attributes, whereCondition)}
  `;

  const resultSet = await executeQuery(uri, sql);

  return resultSet;
}

/**
 * Destroy view from given data source
 *
 * @param {string} uri Database URI
 * @param {string} viewName View name to create
 */
async function destroyView(uri, viewName) {
  const sql = `DROP VIEW IF EXISTS ${viewName};`;

  const resultSet = await executeQuery(uri, sql);

  return resultSet;
}
// Internal View Module
const viewModule = { createView, destroyView, validateView, EmptyViewError };

module.exports = viewModule;