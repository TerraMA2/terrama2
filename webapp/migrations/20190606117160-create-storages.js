module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('storages', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: Sequelize.STRING
      },
      description: Sequelize.TEXT,
      active: Sequelize.BOOLEAN,
      erase_all: Sequelize.BOOLEAN,
      keep_data: Sequelize.INTEGER,
      keep_data_unit:  Sequelize.TEXT,
      filter: Sequelize.BOOLEAN,
      schedule_type: Sequelize.INTEGER,
      backup: Sequelize.BOOLEAN,
      uri: Sequelize.TEXT,
      zip: Sequelize.BOOLEAN,
      project_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'projects',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      data_provider_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_providers',
            schema: "terrama2"
          },
        },
        allowNull: true,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      data_series_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_series',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      schedule_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'schedules',
            schema: "terrama2"
          },
        },
        allowNull: true,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      automatic_schedule_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'automatic_schedules',
            schema: "terrama2"
          },
        },
        allowNull: true,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      service_instance_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'service_instances',
            schema: "terrama2"
          },
        },
        allowNull: true,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'storages', schema: 'terrama2' });
  }
};