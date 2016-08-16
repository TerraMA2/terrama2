module.exports = function(sequelize, DataTypes) {
  "use strict";

  var AnalysisAreaOfInterestType = sequelize.define("AnalysisAreaOfInterestType",
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

        classMethods: {
          associate: function(models) {
            AnalysisAreaOfInterestType.hasMany(models.AnalysisOutputGrid, {
              onDelete: "CASCADE",
              foreignKey: {
                name: 'area_of_interest_type',
                allowNull: false
              }
            });
          }
        }
      }
  );

  return AnalysisAreaOfInterestType;
};
