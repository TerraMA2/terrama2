var Sequelize = require('sequelize');

module.exports = function(config) {
  return new Sequelize(config.database, config.username, config.password, config);
};