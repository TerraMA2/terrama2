module.exports = function(sequelize, DataTypes) {
  return sequelize.define("DataProviderType",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        unique: true
      },
      description: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,

      classMethods: {
        associate: function(models) {

        }
      }
    }
  );
};