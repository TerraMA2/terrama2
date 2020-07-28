module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('value_comparison_operations', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      frequency: Sequelize.INTEGER,
      frequency_unit: Sequelize.STRING,
      schedule: Sequelize.TIME,
      schedule_retry: Sequelize.INTEGER,
      schedule_retry_unit: Sequelize.STRING,
      schedule_timeout: Sequelize.INTEGER,
      schedule_timeout_unit: Sequelize.STRING,
      value_comparison_operation: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'filters',
            schema: "terrama2"
          },
        },
        allowNull: false,
        unique: true,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'value_comparison_operations', schema: 'terrama2' });
  }
};