module.exports = function(sequelize, DataTypes) {
  var Schedule = sequelize.define("Schedule",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      frequency: DataTypes.INTEGER,
      frequency_unit: DataTypes.STRING,
      frequency_start_time: DataTypes.STRING,
      schedule: DataTypes.INTEGER,
      schedule_time: DataTypes.STRING,
      schedule_unit: DataTypes.STRING,
      schedule_retry: DataTypes.INTEGER,
      schedule_retry_unit: DataTypes.STRING,
      schedule_timeout: DataTypes.INTEGER,
      schedule_timeout_unit: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          Schedule.hasMany(models.Collector, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          Schedule.hasOne(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'schedule_id',
              allowNull: false
            }
          });

          Schedule.hasOne(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'schedule_id',
              allowNull: true,
              constraints: true
            }
          });

          Schedule.hasOne(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'schedule_id',
              allowNull: true,
              constraints: true
            }
          });

          Schedule.hasOne(models.ReprocessingHistoricalData, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "schedule_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Schedule;
};
