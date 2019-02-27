const DataManager = require('./../DataManager');

/** Exception used when the SELECT returned 0 */
class EmptyViewError extends Error { };

/**
 * This function returns SELECT statement used to generate View
 *
 * @param {string} tableName Origin table name
 * @param {string[]} attributes List of attributes from origin table
 * @param {string} whereCondition Query restriction
 */
const prepareSelect = (tableName, attributes = [], whereCondition = '') => {
  if (!attributes || attributes.length === 0)
    attributes = ['*'];

  if (!whereCondition)
    whereCondition = '1=1';

  return `SELECT ${attributes.join(',')} FROM ${tableName} WHERE ${whereCondition}`;
}

/**
 * Performs view validation, checking if SELECT statements returns at least a row data set
 *
 * @throws {EmptyViewError|Error} when result is empty
 * @param {string} tableName Origin table name
 * @param {string[]} attributes List of attributes from origin table
 * @param {string} whereCondition Query restriction
 */
async function validateView(tableName, attributes, whereCondition) {
  const sql = prepareSelect(tableName, attributes, whereCondition);

  const resultSet = await DataManager.orm.query(sql);

  if (resultSet.length === 0)
    throw new EmptyViewError(`The view result is empty. Make sure the query builder has been set properly`);

  return resultSet;
}

/**
 * Tries to create view from dataset
 *
 * @param {string} viewName Destination view name
 * @param {string} tableName Origin table name
 * @param {string[]} attributes List of attributes from origin table
 * @param {string} whereCondition Query restriction
 */
async function createView(viewName, tableName, attributes = [], whereCondition = '1=1') {
  const sql = `
    DROP VIEW IF EXISTS ${viewName};
    CREATE OR REPLACE VIEW ${viewName} AS
      ${prepareSelect(tableName, attributes, whereCondition)}
  `;

  const resultSet = await DataManager.orm.query(sql);

  return resultSet;
}

const viewModule = { createView, validateView, EmptyViewError };

module.exports = viewModule;