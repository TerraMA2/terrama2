module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('schedules', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      frequency: Sequelize.INTEGER,
      frequency_unit: Sequelize.STRING,
      frequency_start_time: Sequelize.STRING,
      schedule: Sequelize.INTEGER,
      schedule_time: Sequelize.STRING,
      schedule_unit: Sequelize.STRING,
      schedule_retry: Sequelize.INTEGER,
      schedule_retry_unit: Sequelize.STRING,
      schedule_timeout: Sequelize.INTEGER,
      schedule_timeout_unit: Sequelize.STRING
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'schedules', schema: 'terrama2' });
  }
};