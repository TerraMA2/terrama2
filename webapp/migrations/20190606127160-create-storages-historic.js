module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('storages_historic', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      process_id: {
        type: Sequelize.INTEGER,
        allowNull: false
      },
        status: {
        type: Sequelize.INTEGER,
        allowNull: false
      },
      start_timestamp: Sequelize.DATE,
      data_timestamp: Sequelize.DATE,
      last_process_timestamp: Sequelize.DATE,
      data: Sequelize.TEXT,
      origin: Sequelize.TEXT,
      type: Sequelize.INTEGER,
      description: Sequelize.TEXT,
      storage_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'storages',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'storages_historic', schema: 'terrama2' });
  }
};