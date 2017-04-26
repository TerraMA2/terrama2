module.exports = function(sequelize, DataTypes) {
  var DataProviderOptions = sequelize.define("DataProviderOptions",
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
          DataProviderOptions.belongsTo(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataProviderOptions;
};
