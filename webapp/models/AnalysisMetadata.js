module.exports = function(sequelize, DataTypes) {
  var AnalysisMetadata = sequelize.define("AnalysisMetadata",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: DataTypes.STRING,
      value: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      tableName: 'analysis_metadata',

      classMethods: {
        associate: function(models) {
          AnalysisMetadata.belongsTo(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'analysis_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AnalysisMetadata;
};