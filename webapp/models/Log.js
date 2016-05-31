module.exports = function(sequelize, DataTypes) {
  var Log = sequelize.define("Log",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      host: {
        type: DataTypes.STRING,
        allowNull: false
      },
      port: {
        type: DataTypes.INTEGER,
        allowNull: false
      },
      user: {
        type: DataTypes.STRING,
        allowNull: false
      },
      password: DataTypes.STRING,
      database: {
        type: DataTypes.STRING,
        allowNull: false
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          Log.belongsTo(models.ServiceInstance, {
            foreignKey: {
              allowNull: false,
              unique: true
            }
          });
        }
      }
    }
  );

  return Log;
};