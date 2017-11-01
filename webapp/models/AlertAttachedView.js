module.exports = function(sequelize, DataTypes) {
  var AlertAttachedView = sequelize.define("AlertAttachedView",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      layer_order: {
        type: DataTypes.INTEGER,
        allowNull: false
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      classMethods: {
        associate: function(models) {
          AlertAttachedView.belongsTo(models.AlertAttachment, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'alert_attachment_id',
              allowNull: false
            }
          });

          AlertAttachedView.belongsTo(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'view_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return AlertAttachedView;
};
