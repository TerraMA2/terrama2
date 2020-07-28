module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('interpolation_method', {
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
    return queryInterface.dropTable({ tableName: 'interpolation_method', schema: 'terrama2' });
  }
};