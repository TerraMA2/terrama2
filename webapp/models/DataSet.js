module.exports = function(sequelize, DataTypes) {
  var DataSet = sequelize.define("DataSet",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        unique: true
      },
      active: DataTypes.BOOLEAN
    },
    {
      underscored: true,
      underscoredAll: true,

      classMethods: {
        associate: function(models) {
          DataSet.belongsTo(models.DataSeries, {
            foreignKey: {
              onDelete: "CASCADE",
              foreignKey: {
                allowNull: false
              }
            }
          });
        }
      }
    }
  );

  return DataSet;
};