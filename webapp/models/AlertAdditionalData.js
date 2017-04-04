module.exports = function(sequelize, DataTypes) {
  var AlertAdditionalData = sequelize.define("AlertAdditionalData",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      referrer_attribute: DataTypes.STRING,
      referred_attribute: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          AlertAdditionalData.belongsTo(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          AlertAdditionalData.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "dataseries_id",
              allowNull: false
            }
          });

          AlertAdditionalData.belongsTo(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_set_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AlertAdditionalData;
};
