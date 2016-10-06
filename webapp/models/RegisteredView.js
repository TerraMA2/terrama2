(function() {
  'use strict';

  /**
   * 
   * @param {Sequelize} sequelize - ORM instance
   * @param {Sequelize.DataTypes} DataTypes - ORM database field types
   */
  module.exports = function(sequelize, DataTypes) {
    var RegisteredView = sequelize.define("RegisteredView", 
      {
        id: {
          type: DataTypes.INTEGER,
          allowNull: false,
          primaryKey: true,
          autoIncrement: true
        },
        workspace: {
          type: DataTypes.STRING,
          allowNull: false,
          comment: "Map server workspace"
        },
        uri: {
          type: DataTypes.STRING,
          allowNull: false,
          comment: "Map server URI"
        }
      },
      {
        underscored: true,
        underscoredAll: true,
        timestamps: true,

        classMethods: {
          associate: function(models) {
            RegisteredView.hasMany(models.Layer, {
              onDelete: "CASCADE",
              foreignKey: {
                allowNull: false
              }
            });

            RegisteredView.belongsTo(models.View, {
              onDelete: "CASCADE",
              foreignKey: {
                name: "view_id",
                allowNull: false
              }
            });
          }
        }
      }
    );
    return RegisteredView;
  };
} ());