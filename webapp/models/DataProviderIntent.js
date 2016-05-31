module.exports = function(sequelize, DataTypes) {
  var DataProviderIntent = sequelize.define("DataProviderIntent",
    {
      id: {
       type: DataTypes.INTEGER,
       allowNull: false,
       primaryKey: true,
       autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        unique: true,
        primaryKey: true
      },
      description: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataProviderIntent.hasMany(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataProviderIntent;
};