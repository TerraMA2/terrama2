module.exports = function(sequelize, DataTypes) {
  "use strict";

  var InterpolationMethod = sequelize.define("InterpolationMethod",
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
        description: DataTypes.TEXT
      },
      {
        underscored: true,
        underscoredAll: true,
        timestamps: false,
        tableName: 'interpolation_method',

        classMethods: {
          associate: function(models) {
            InterpolationMethod.hasMany(models.AnalysisOutputGrid, {
              onDelete: "CASCADE",
              foreignKey: {
                name: 'interpolation_method',
                allowNull: false
              }
            });
          }
        }
      }
  );

  return InterpolationMethod;
};