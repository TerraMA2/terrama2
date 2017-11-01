module.exports = function(sequelize, DataTypes) {
  var AlertAttachment = sequelize.define("AlertAttachment",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      y_max: {
        type: DataTypes.DOUBLE,
        allowNull: false
      },
      y_min: {
        type: DataTypes.DOUBLE,
        allowNull: false
      },
      x_max: {
        type: DataTypes.DOUBLE,
        allowNull: false
      },
      x_min: {
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