module.exports = function(sequelize, DataTypes) {
  var AnalysisDataSeriesMetadata = sequelize.define("AnalysisDataSeriesMetadata",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: {
        type: DataTypes.STRING,
        allowNull: false
      },
      value: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      tableName: 'analysis_metadata',

      classMethods: {
        associate: function(models) {
          AnalysisDataSeriesMetadata.belongsTo(models.AnalysisDataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'analysis_data_series_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AnalysisDataSeriesMetadata;
};