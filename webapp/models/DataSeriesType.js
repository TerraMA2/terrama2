module.exports = function(sequelize, DataTypes) {
  var DataSeriesType = sequelize.define("DataSeriesType",
    {
      name: {
        type: DataTypes.STRING,
        allowNull: false,
        primaryKey: true
      },
      description: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataSeriesType.hasMany(models.DataSeriesSemantic, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSeriesType;
};