module.exports = function(sequelize, DataTypes) {
  "use strict";

  var AnalysisOutputGrid = sequelize.define("AnalysisOutputGrid",
      {
        interpolation_dummy: {
          type: DataTypes.NUMERIC
        },
        resolution_x: {
          type: DataTypes.NUMERIC
        },
        resolution_y: {
          type: DataTypes.NUMERIC
        },
        srid: {
          type: DataTypes.INTEGER
        },
        area_of_interest_box: {
          type: DataTypes.GEOMETRY("Polygon")
        }
      },
      {
        underscored: true,
        underscoredAll: true,
        timestamps: false,

        classMethods: {
          associate: function(models) {
            AnalysisOutputGrid.belongsTo(models.Analysis, {
              onDelete: "CASCADE",
              foreignKey: {
                name: 'analysis_id',
                allowNull: false
              }
            });

            AnalysisOutputGrid.belongsTo(models.AnalysisResolutionType, {
              onDelete: "CASCADE",
              foreignKey: {
                name: 'resolution_type',
                allowNull: false
              }
            });

            AnalysisOutputGrid.belongsTo(models.InterpolationMethod, {
              onDelete: "CASCADE",
              foreignKey: {
                name: 'interpolation_method',
                allowNull: false
              }
            });

            AnalysisOutputGrid.belongsTo(models.AnalysisAreaOfInterestType, {
              onDelete: "CASCADE",
              foreignKey: {
                name: 'area_of_interest_type',
                allowNull: false
              }
            });

            AnalysisOutputGrid.belongsTo(models.DataSeries, {
              onDelete: "CASCADE",
              foreignKey: {
                name: "resolution_data_series_id",
                allowNull: true
              }
            });

            AnalysisOutputGrid.belongsTo(models.DataSeries, {
              onDelete: "CASCADE",
              foreignKey: {
                name: "area_of_interest_data_series_id",
                allowNull: true
              }
            });
          }
        }
      }
  );

  return AnalysisOutputGrid;
};
