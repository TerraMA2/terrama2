(function() {
  'use strict';

  /**
   * @param {Sequelize} sequelize - ORM instance
   */
  module.exports = function(sequelize, DataTypes) {
    var Layer = sequelize.define("Layer", {
        id: {
          type: DataTypes.INTEGER,
          allowNull: false,
          primaryKey: true,
          autoIncrement: true
        },
        name: {
          type: DataTypes.STRING,
          allowNull: false,
        }
      },
      {
        underscored: true,
        underscoredAll: true,
        timestamps: false,

        classMethods: {
          associate: function(models) {
            Layer.belongsTo(models.RegisteredView, {
              onDelete: "CASCADE",
              foreignKey: {
                allowNull: false
              }
            });
          }
        }
      }
    );

    return Layer;
  };
} ());