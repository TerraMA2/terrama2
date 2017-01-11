module.exports = function(sequelize, DataTypes) {
  var ViewStyleType = sequelize.define("ViewStyleType",
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
      comment: "It defines a list of all styles ramp types supported by TerraMA2². ",
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          ViewStyleType.hasMany(models.ViewStyleLegend, {
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

  return ViewStyleType;
};