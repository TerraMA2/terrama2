module.exports = function(sequelize, DataTypes) {
  "use strict";

  var AnalysisResolutionType = sequelize.define("AnalysisResolutionType",
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
        description: DataTypes.TEXT
      },
      {
        underscored: true,
        underscoredAll: true,
        timestamps: false,

        classMethods: {
          associate: function(models) {
            AnalysisResolutionType.hasMany(models.AnalysisOutputGrid, {
              onDelete: "CASCADE",
              foreignKey: {
                name: 'resolution_type',
                allowNull: false
              }
            });
          }
        }
      }
  );

  return AnalysisResolutionType;
};
