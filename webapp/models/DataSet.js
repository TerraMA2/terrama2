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
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataSet.belongsTo(models.DataSeries, {
            foreignKey: {
              name: "data_series_id",
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