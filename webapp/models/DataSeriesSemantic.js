module.exports = function(sequelize, DataTypes) {
  var DataSeriesSemantic = sequelize.define("DataSeriesSemantic",
    {
      name: {
        type: DataTypes.STRING,
        allowNull: false,
        primaryKey: true
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataSeriesSemantic.belongsTo(models.DataSeriesType, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSeriesSemantic.belongsTo(models.DataFormat, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSeriesSemantic.hasMany(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSeriesSemantic;
};