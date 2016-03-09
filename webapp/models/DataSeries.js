module.exports = function(sequelize, DataTypes) {
  var DataSeries = sequelize.define("DataSeries",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        unique: true
      },
      description: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,

      classMethods: {
        associate: function(models) {
          DataSeries.belongsTo(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSeries.belongsTo(models.DataSeriesSemantic, {
            onDelete: "CASCADE",
            foreignKey: {
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

          DataSeries.hasMany(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSeries;
};