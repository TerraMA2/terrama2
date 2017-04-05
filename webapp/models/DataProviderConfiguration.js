module.exports = function(sequelize, DataTypes) {
  var DataProviderConfiguration = sequelize.define("DataProviderConfiguration",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: DataTypes.STRING,
      value: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataProviderConfiguration.belongsTo(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataProviderConfiguration;
};
