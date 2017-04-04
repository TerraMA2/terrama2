module.exports = function(sequelize, DataTypes) {
  var RiskLevel = sequelize.define("RiskLevel",
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
          RiskLevel.belongsTo(models.Risk, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false,
              name: 'risk_id',
            }
          });
        }
      }
    }
  );

  return RiskLevel;
};
