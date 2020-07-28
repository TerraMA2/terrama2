module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('automatic_schedules', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      data_ids: Sequelize.ARRAY(Sequelize.INTEGER)
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'automatic_schedules', schema: 'terrama2' });
  }
};