module.exports = function(sequelize, DataTypes) {
  var ScriptLanguage = sequelize.define("ScriptLanguage",
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
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          ScriptLanguage.hasMany(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "script_language_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return ScriptLanguage;
};
