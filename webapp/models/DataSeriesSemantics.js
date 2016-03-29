module.exports = function(sequelize, DataTypes) {
  var DataSeriesSemantics = sequelize.define("DataSeriesSemantics",
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
          DataSeriesSemantics.belongsTo(models.DataSeriesType, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSeriesSemantics.belongsTo(models.DataFormat, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSeriesSemantics.hasMany(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSeriesSemantics;
};