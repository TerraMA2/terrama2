var bcrypt = require('bcrypt');

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
      },
      token: {
        type: DataTypes.STRING,
        allowNull: false
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      classMethods: {
        associate: function(models){
          User.hasMany(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'user_id',
              allowNull: false
            }
          });
        },
        generateSalt: function() {
          return bcrypt.genSaltSync(10);
        },
        generateHash: function(password, salt) {
          return bcrypt.hashSync(password, salt);
        },
        generateToken: function(userString) {
          return bcrypt.hashSync(userString, 10);
        }
      }
    }
  );

  return User;
};
