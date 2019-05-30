module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_collection_logs', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      origin_uri: Sequelize.STRING,
      storage_uri: Sequelize.STRING,
      data_timestamp: Sequelize.TIME,
      collect_timestamp: Sequelize.TIME,
      collector_input_output_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'collector_input_outputs',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      status_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'log_status',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'data_collection_logs', schema: 'terrama2' });
  }
};