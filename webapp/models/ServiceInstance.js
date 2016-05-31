module.exports = function(sequelize, DataTypes) {
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
      runEnviroment: DataTypes.STRING
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
        }
      }
    }
  );

  return ServiceInstance;
};