module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('analysis_area_of_interest_types', {
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
    return queryInterface.dropTable({ tableName: 'analysis_area_of_interest_types', schema: 'terrama2' });
  }
};