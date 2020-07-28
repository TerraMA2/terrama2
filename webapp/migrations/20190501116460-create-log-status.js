module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('log_status', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      status: Sequelize.STRING
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'log_status', schema: 'terrama2' });
  }
};