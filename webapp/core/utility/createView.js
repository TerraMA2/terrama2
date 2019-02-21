const DataManager = require('./../DataManager');

class EmptyViewError extends Error { };

const prepareSelect = (tableName, attributes = [], whereCondition = '') => {
  if (!attributes || attributes.length === 0)
    attributes = ['*'];

  if (!whereCondition)
    whereCondition = '1=1';

  return `SELECT ${attributes.join(',')} FROM ${tableName} WHERE ${whereCondition}`;
}

async function validateView(tableName, attributes, whereCondition) {
  const sql = prepareSelect(tableName, attributes, whereCondition);

  const resultSet = await DataManager.orm.query(sql);

  if (resultSet.length === 0)
    throw new EmptyViewError(`The view result is empty. Make sure the query builder has been set properly`);

  return resultSet;
}

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