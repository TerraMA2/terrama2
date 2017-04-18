module.exports = function(sequelize, DataTypes) {
  var AlertNotification = sequelize.define("AlertNotification",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      include_report: DataTypes.STRING,
      notify_on_change: DataTypes.BOOLEAN,
      simplified_report: DataTypes.BOOLEAN,
      notify_on_risk_level: DataTypes.INTEGER,
      recipients: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          AlertNotification.belongsTo(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AlertNotification;
};
