module.exports = function(sequelize, DataTypes) {
  var DataSetGrid = sequelize.define("DataSetGrid",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataSetGrid.belongsTo(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSetGrid;
};
