module.exports = function(sequelize, DataTypes) {
  var ServiceType = sequelize.define("ServiceType", {
    id: {
      type: DataTypes.INTEGER,
      allowNull: false,
      primaryKey: true,
      autoIncrement: true
    },
    name: {
      type: DataTypes.STRING,
      unique: true
    }
  },
  {
    underscored: true,
    underscoredAll: true,
    classMethods: {
      associate: function(models) {
        ServiceType.hasMany(models.ServiceInstance, {
          onDelete: "CASCADE",
          foreignKey: {
            type: DataTypes.INTEGER,
            allowNull: false
          }
        });
      }
    }
  });

  return ServiceType;
};