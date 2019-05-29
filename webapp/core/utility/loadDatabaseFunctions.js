const fs = require('fs');
const { resolve: pathResolver } = require('path');
const logger = require('./../Logger');
const { Connection } = require('./connection');


/**
 * Load TerraMA² SQL functions into database
 *
 * @param {string} uri Database URI
 */
async function loadDatabaseFunctions(uri) {
  const connector = new Connection(uri);

  await connector.connect();

  let sqlFiles = null;

  const scriptsDirectory = pathResolver(__dirname, '../../../share/terrama2/scripts/sql-functions');

  try {
    sqlFiles = fs.readdirSync(scriptsDirectory).filter(file => file.endsWith('.sql'));
  } catch (err) {
    logger.error(`Could not read directory "${scriptsDirectory}". ${err.code}. No SQL functions loaded`);
    throw err;
  }

  for(let sqlFile of sqlFiles) {
    const sql = fs.readFileSync(pathResolver(scriptsDirectory, sqlFile));

    await connector.execute(sql);
  }

  await connector.disconnect();
}

module.exports = loadDatabaseFunctions;