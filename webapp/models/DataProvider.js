module.exports = function(sequelize, DataTypes) {
  var DataProvider = sequelize.define("DataProvider", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      uri: DataTypes.STRING,
      name: {
        type: DataTypes.STRING
      },
      description: DataTypes.TEXT,
      active: {
        type: DataTypes.BOOLEAN,
        allowNull: false
      }
    },
    {
      indexes: [
        {
          unique: true,
          fields: ['project_id', 'name']
        }
      ],
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      hooks: {
        afterCreate: function(instance, options) {
          // insert it to datamanager
        },

        afterDestroy: function(instance, options) {
          // remove it from datamanager
        }
      },
      classMethods: {
        associate: function(models) {
          DataProvider.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });

          DataProvider.belongsTo(models.DataProviderType, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataProvider.belongsTo(models.DataProviderIntent, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataProvider.hasMany(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataProvider.hasMany(models.DataProviderOptions, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  DataProvider.schema('terrama2');

  return DataProvider;
};