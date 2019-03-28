module.exports = function(sequelize, DataTypes) {
  "use strict";

  var storages = sequelize.define("storages",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING
      },
      description: DataTypes.TEXT,
      active: DataTypes.BOOLEAN,
      erase_all: DataTypes.BOOLEAN,
      keep_data: DataTypes.INTEGER,
      keep_data_unit:  DataTypes.TEXT,
      filter: DataTypes.BOOLEAN,
      schedule_type: DataTypes.INTEGER,
      backup: DataTypes.BOOLEAN,
      uri: DataTypes.TEXT,
      zip: DataTypes.BOOLEAN
   },
   {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          storages.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });

          storages.belongsTo(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_provider_id",
              allowNull: true
            }
          });

          storages.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });

          storages.belongsTo(models.Schedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "schedule_id",
              allowNull: true,
              constraints: true
            }
          });

          storages.belongsTo(models.AutomaticSchedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "automatic_schedule_id",
              allowNull: true,
              constraints: true
            }
          });

          storages.belongsTo(models.ServiceType, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "service_type_id",
              allowNull: true,
              constraints: true
            }
          });

          storages.belongsTo(models.ServiceInstance, {
            foreignKey: {
              allowNull: true
            }
          });
        }
      }
    }
  );

  return storages;
};
