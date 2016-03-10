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
      timestamps: false,

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
              name: "data_series_id",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.Collector, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_input_id",
              allowNull: false
            }
          });

          DataSeries.hasMany(models.Collector, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_output_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSeries;
};