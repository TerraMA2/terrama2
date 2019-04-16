module.exports = function(sequelize, DataTypes) {
  var Storages = sequelize.define("Storages",
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
          Storages.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });

          Storages.belongsTo(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_provider_id",
              allowNull: true
            }
          });

          Storages.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });

          Storages.belongsTo(models.Schedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "schedule_id",
              allowNull: true,
              constraints: true
            }
          });

          Storages.belongsTo(models.AutomaticSchedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "automatic_schedule_id",
              allowNull: true,
              constraints: true
            }
          });

          Storages.belongsTo(models.ServiceInstance, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "service_instance_id",
              allowNull: true,
              constraints: true
            }
          });

        }
      }
    }
  );

  return Storages;
};
