module.exports = function(sequelize, DataTypes) {
  var Risk = sequelize.define("Risk",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          Risk.belongsTo(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          Risk.hasMany(models.RiskLevel, {
            onDelete: 'CASCADE',
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Risk;
};
