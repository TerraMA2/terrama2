module.exports = function(sequelize, DataTypes) {
  var Risk = sequelize.define("Risk",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: DataTypes.STRING,
      description: DataTypes.TEXT,
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          Risk.hasOne(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'risk_id',
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
