module.exports = function(sequelize, DataTypes) {
  var AlertAttachment = sequelize.define("AlertAttachment",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      maxy: {
        type: DataTypes.DOUBLE,
        allowNull: false
      },
      miny: {
        type: DataTypes.DOUBLE,
        allowNull: false
      },
      maxx: {
        type: DataTypes.DOUBLE,
        allowNull: false
      },
      minx: {
        type: DataTypes.DOUBLE,
        allowNull: false
      },
      srid: {
        type: DataTypes.INTEGER,
        allowNull: false
      },
      width: {
        type: DataTypes.INTEGER,
        allowNull: true
      },
      height: {
        type: DataTypes.INTEGER,
        allowNull: true
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      classMethods: {
        associate: function(models) {
          AlertAttachment.belongsTo(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'alert_id',
              allowNull: false
            }
          });

          AlertAttachment.hasMany(models.AlertAttachedView, {
            onDelete: 'CASCADE',
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AlertAttachment;
};