module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('analysis_resolution_types', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true
      },
      name: {
        type: Sequelize.STRING,
        allowNull: false
      },
      description: Sequelize.TEXT
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'analysis_resolution_types', schema: 'terrama2' });
  }
};