module.exports = function(sequelize, DataTypes) {
  "use strict";

  var DataSeries = sequelize.define("DataSeries",
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
      active: DataTypes.BOOLEAN
    },
    {
      indexes: [
        {
          unique: true,
          fields: ['project_id', 'name']
        }
      ],
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataSeries.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });

          DataSeries.belongsTo(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSeries.belongsTo(models.DataSeriesSemantics, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_semantics_id",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.DataSeriesProperty, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.Intersection, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'dataseries_id',
              allowNull: false
            }
          });

          DataSeries.hasMany(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.AnalysisDataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.AnalysisOutputGrid, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "resolution_data_series_id",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.AnalysisOutputGrid, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "area_of_interest_data_series_id",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.Interpolator, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_input",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.Interpolator, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_output",
              allowNull: false
            }
          });

          DataSeries.hasOne(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });

          DataSeries.hasOne(models.AlertAdditionalData, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "dataseries_id",
              allowNull: false
            }
          });

          DataSeries.hasOne(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });

          DataSeries.hasOne(models.Filter, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: true
            }
          });
        }
      }
    }
  );

  return DataSeries;
};
