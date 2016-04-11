module.exports = function(sequelize, DataTypes) {
  var DataSetDcp = sequelize.define("DataSetDcp",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      position: DataTypes.GEOMETRY('Point', 4326)
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataSetDcp.belongsTo(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSetDcp;
};