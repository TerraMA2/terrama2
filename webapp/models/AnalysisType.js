module.exports = function(sequelize, DataTypes) {
  var AnalysisType = sequelize.define("AnalysisType",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        allowNull: false,
        unique: true
      },
      description: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          AnalysisType.hasMany(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'type_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AnalysisType;
};