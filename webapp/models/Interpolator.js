module.exports = function(sequelize, DataTypes) {
  var Interpolator = sequelize.define("Interpolator",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      active: DataTypes.BOOLEAN,
      schedule_type: DataTypes.INTEGER,
      bounding_rect: DataTypes.TEXT,
      interpolation_attribute: DataTypes.STRING,
      resolution_x: DataTypes.DECIMAL,
      resolution_y: DataTypes.DECIMAL,
      srid: DataTypes.INTEGER
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {

          Interpolator.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });

          Interpolator.belongsTo(models.ServiceInstance, {
            foreignKey: {
              allowNull: true
            }
          });

          Interpolator.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            as: "dataSeriesInput",
            foreignKey: {
              name: "data_series_input",
              allowNull: false
            }
          });

          Interpolator.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            as: "dataSeriesOutput",
            foreignKey: {
              name: "data_series_output",
              allowNull: false
            }
          });

          Interpolator.belongsTo(models.Schedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "schedule_id",
              allowNull: true,
              constraints: true
            }
          });

          Interpolator.belongsTo(models.AutomaticSchedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "automatic_schedule_id",
              allowNull: true,
              constraints: true
            }
          });

          Interpolator.belongsTo(models.InterpolatorStrategy, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'interpolator_strategy',
              allowNull: false
            }
          });

          Interpolator.hasMany(models.InterpolatorMetadata, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'interpolator_id',
              allowNull: false
            }
          });

        }
      }
    }
  );

  return Interpolator;
};
