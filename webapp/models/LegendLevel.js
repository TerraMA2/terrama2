module.exports = function(sequelize, DataTypes) {
  var LegendLevel = sequelize.define("LegendLevel",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: DataTypes.STRING,
      value: DataTypes.DOUBLE,
      level: DataTypes.INTEGER.UNSIGNED
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          LegendLevel.belongsTo(models.Legend, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false,
              name: 'legend_id',
            }
          });
        }
      }
    }
  );

  return LegendLevel;
};
