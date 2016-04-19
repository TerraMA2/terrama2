module.exports = function(sequelize, DataTypes) {
  var User = sequelize.define(
    "User",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        allowNull: false
      },
      username: {
        type: DataTypes.STRING,
        allowNull: false,
        unique: true
      },
      password: {
        type: DataTypes.STRING,
        allowNull: false
      },
      salt: {
        type: DataTypes.STRING,
        allowNull: false
      },
      cellphone: {
        type: DataTypes.STRING,
        allowNull: false
      },
      email: {
        type: DataTypes.STRING,
        allowNull: false
      },
      administrator: {
        type: DataTypes.BOOLEAN,
        allowNull: false
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false
    }
  );

  User.schema('terrama2');

  return User;
};
