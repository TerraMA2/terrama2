module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_series_types', {
      name: {
        type: Sequelize.STRING,
        allowNull: false,
        primaryKey: true
      },
      description: Sequelize.STRING
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'data_series_types', schema: 'terrama2' });
  }
};