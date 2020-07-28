module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('interpolator_strategy', {
      id: {
        type: Sequelize.STRING,
        allowNull: false,
        primaryKey: true
      },
      name: {
        type: Sequelize.STRING,
        allowNull: false,
        unique: true
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'interpolator_strategy', schema: 'terrama2' });
  }
};