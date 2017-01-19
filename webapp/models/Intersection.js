module.exports = function(sequelize, DataTypes) {
  var Intersection = sequelize.define("Intersection",
    {
      id: {
       type: DataTypes.INTEGER,
       allowNull: false,
       primaryKey: true,
       autoIncrement: true
      },
      attribute: {
        type: DataTypes.STRING
      },
      alias: {
        type: DataTypes.STRING
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          Intersection.belongsTo(models.Collector, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'collector_id',
              allowNull: false
            }
          });

          Intersection.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'dataseries_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Intersection;
};
