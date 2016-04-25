module.exports = function(sequelize, DataTypes) {
  var AnalysisDataSeriesType = sequelize.define("AnalysisDataSeriesType",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        allowNull: false
      },
      description: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      tableName: 'analysis_metadata',

      classMethods: {
        associate: function(models) {
          AnalysisDataSeriesType.hasMany(models.AnalysisDataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'type_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AnalysisDataSeriesType;
};