module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.sequelize.query(`CREATE EXTENSION IF NOT EXISTS postgis`)
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.sequelize.query(`DROP EXTENSION IF EXISTS postgis`)
  }
};