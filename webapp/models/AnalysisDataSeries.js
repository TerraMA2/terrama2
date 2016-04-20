module.exports = function(sequelize, DataTypes) {
  var AnalysisDataSeries = sequelize.define("AnalysisDataSeries",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      alias: {
        type: DataTypes.STRING,
        allowNull: false
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      tableName: 'analysis_metadata',

      classMethods: {
        associate: function(models) {
          AnalysisDataSeries.hasMany(models.AnalysisDataSeriesMetadata, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'analysis_data_series_id',
              allowNull: false
            }
          });

          AnalysisDataSeries.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'data_series_id',
              allowNull: false
            }
          });

          AnalysisDataSeries.belongsTo(models.AnalysisDataSeriesType, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "type_id",
              allowNull: false
            }
          });

          AnalysisDataSeries.belongsTo(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "analysis_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AnalysisDataSeries;
};