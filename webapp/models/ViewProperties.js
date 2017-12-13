module.exports = function(sequelize, DataTypes) {
  var ViewProperties = sequelize.define("ViewProperties",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: DataTypes.STRING,
      value: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          ViewProperties.belongsTo(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "view_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return ViewProperties;
};