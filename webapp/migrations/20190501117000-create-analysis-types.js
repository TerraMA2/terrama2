module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('analysis_types', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: Sequelize.STRING,
        allowNull: false,
        unique: true
      },
      description: Sequelize.STRING
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'analysis_types', schema: 'terrama2' });
  }
};