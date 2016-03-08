module.exports = function(sequelize, DataTypes) {
  var DataFormat = sequelize.define("DataFormat",
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

      classMethods: {
        associate: function(models) {
          DataFormat.hasMany(models.DataSeriesSemantic, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          //DataFormat.belongsToMany(models.DataSeriesType, {
          //  through: models.DataSeriesSemantic,
          //  onDelete: "CASCADE",
          //  foreignKey: {
          //    allowNull: false
          //  }
          //});
        }
      }
    }
  );

  return DataFormat;
};