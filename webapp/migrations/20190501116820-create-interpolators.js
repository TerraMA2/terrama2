module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('interpolators', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      active: Sequelize.BOOLEAN,
      schedule_type: Sequelize.INTEGER,
      bounding_rect: Sequelize.TEXT,
      interpolation_attribute: Sequelize.STRING,
      resolution_x: Sequelize.DECIMAL,
      resolution_y: Sequelize.DECIMAL,
      srid: Sequelize.INTEGER,
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
      service_instance_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'service_instances',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'SET NULL',
        onUpdate: 'CASCADE'
      },
      data_series_input: {
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
      data_series_output: {
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
        allowNull: false,
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
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      interpolator_strategy: {
        type: Sequelize.STRING,
        references: {
          key: 'id',
          model: {
            tableName: 'interpolator_strategy',
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
    return queryInterface.dropTable({ tableName: 'interpolators', schema: 'terrama2' });
  }
};