/**
 * It exports a sequelize orm Model in order to load and to associate within another orm models.
 * @returns {sequelize.Model}
 */
module.exports = function(sequelize, DataTypes) {
  'use strict';

  /**
   * It defines a TerraMA² Service.
   *
   * @type {sequelize.Model}
   */
  var ServiceInstance = sequelize.define("ServiceInstance", {
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
      host: DataTypes.STRING,
      port: DataTypes.INTEGER,
      sshUser: DataTypes.STRING,
      sshPort: {
        type: DataTypes.INTEGER,
        default: 22
      },
      description: DataTypes.TEXT,
      pathToBinary: DataTypes.STRING,
      numberOfThreads: DataTypes.INTEGER,
      runEnviroment: DataTypes.STRING,
      maps_server_uri: {
        type: DataTypes.STRING,
        comment: "Map server connection URI. Used for View Services"
      },
    },
    {
      underscored: true,
      underscoredAll: true,
      classMethods: {
        associate: function(models) {
          ServiceInstance.belongsTo(models.ServiceType, {
            onDelete: "CASCADE",
            foreignKey: {
              type: DataTypes.INTEGER,
              allowNull: false
            }
          });

          ServiceInstance.hasMany(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'instance_id',
              allowNull: false
            }
          });

          ServiceInstance.hasOne(models.Log, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          ServiceInstance.hasMany(models.Collector, {
            foreignKey: {
              allowNull: true
            }
          });

          ServiceInstance.hasMany(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "service_instance_id",
              allowNull: false
            }
          });

          ServiceInstance.hasMany(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "service_instance_id",
              allowNull: false
            }
          });

          ServiceInstance.hasMany(models.ServiceMetadata, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "service_instance_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return ServiceInstance;
};
