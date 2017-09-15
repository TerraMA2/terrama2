module.exports = function(sequelize, DataTypes) {
  "use strict";

  var InterpolatorStrategy = sequelize.define("InterpolatorStrategy",
      {
        id: {
          type: DataTypes.INTEGER,
          allowNull: false,
          primaryKey: true
        },
        name: {
          type: DataTypes.STRING,
          allowNull: false
        },
        code: {
          type: DataTypes.STRING,
          allowNull: false
        }
      },
      {
        underscored: true,
        underscoredAll: true,
        timestamps: false,
        tableName: 'interpolator_strategy',

        classMethods: {
          associate: function(models) {
            InterpolatorStrategy.hasMany(models.Interpolator, {
              onDelete: "CASCADE",
              foreignKey: {
                name: 'interpolator_strategy',
                allowNull: false
              }
            });
          }
        }
      }
  );

  return InterpolationMethod;
};