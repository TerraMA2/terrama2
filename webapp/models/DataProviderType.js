module.exports = function(sequelize, DataTypes) {
  var DataProviderType = sequelize.define("DataProviderType",
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
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataProviderType.hasMany(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataProviderType.hasMany(models.SemanticsProvidersType, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'data_provider_type_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataProviderType;
};
