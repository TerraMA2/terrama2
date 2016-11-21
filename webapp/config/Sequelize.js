var Sequelize = require('sequelize');
var logger = require("./../core/Logger");

/**
 * It defines a Sequelize ORM initialization. It depends TerraMA² database config
 * 
 * @param {Object} config - A database config
 * @param {string} config.database - Database name
 * @param {string} config.username - Database user
 * @param {string} config.password - User password
 * @param {Object?} config.define - Extra database configuration (Sequelize)
 * @returns {Sequelize.Instance}
 */
module.exports = function(config) {
  config.logging = logger.info;
  return new Sequelize(config.database, config.username, config.password, config);
};