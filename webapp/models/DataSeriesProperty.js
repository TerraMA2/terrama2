module.exports = function(sequelize, DataTypes) {
  var DataSeriesProperty = sequelize.define("DataSeriesProperty",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      attribute: DataTypes.STRING,
      alias: DataTypes.STRING,
      position: DataTypes.INTEGER
    },
    {
      underscored: true,
      underscoredAll: true,

      classMethods: {
        associate: function(models) {
          DataSeriesProperty.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSeriesProperty;
};