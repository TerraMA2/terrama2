module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.changeColumn(
      { schema: 'terrama2', tableName: 'views' },
      'automatic_schedule_id',
      {
        type: Sequelize.INTEGER,
        allowNull: true
      },);
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.changeColumn(
      { schema: 'terrama2', tableName: 'views' },
      'automatic_schedule_id',
      {
        type: Sequelize.INTEGER,
        allowNull: false
      });
  }
};