module.exports = function(sequelize, DataTypes) {
  var DataSetFormat = sequelize.define("DataSetFormat",
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

      classMethods: {
        associate: function(models) {
          DataSetFormat.belongsTo(models.DataSet, {
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

  return DataSetFormat;
};