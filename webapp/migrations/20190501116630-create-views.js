module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('views', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "View identifier"
      },
      name: {
        type: Sequelize.STRING,
        allowNull: false,
        comment: "View name"
      },
      description: {
        type: Sequelize.TEXT,
        comment: "View description"
      },
      active: {
        type: Sequelize.BOOLEAN,
        allowNull: false,
        default: true,
        comment: "It defines view can be used and retrieved. Default is true."
      },
      private: {
        type: Sequelize.BOOLEAN,
        allowNull: false,
        default: false,
        comment: "It defines if the view is private. Default is false."
      },
      schedule_type: {
        type: Sequelize.INTEGER,
        allowNull: true
      },
      source_type: {
        type: Sequelize.INTEGER,
        allowNull: false,
        comment: "It defines the type of data source that create the view. Alert, Analysis, Static Data or Dynamic Data"
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
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
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
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'views', schema: 'terrama2' });
  }
};