(function() {
  "use strict";

  /**
   * It represents legend for View
   * 
   * @param {Sequelize} sequelize - Sequelize instance
   * @param {Sequelize.DataTypes} DataTypes - Sequelize Database DataTypes
   * @returns {Sequelize.Model}
   */
  module.exports = function(sequelize, DataTypes) {
    /**
     * It defines a Sequelize View Legend model
     * 
     * @type {Sequelize.Model}
     */
    var ViewStyleLegend = sequelize.define("ViewStyleLegend", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "ViewStyleLegend identifier"
      },
      column: {
        type: DataTypes.STRING,
        allowNull: true,
        comment: "Target column to generate legend"
      }
    }, {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      classMethods: {
        /**
         * It applies a table association during models loading.
         */
        associate: function(models) {
          ViewStyleLegend.belongsTo(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "view_id",
              allowNull: false
            }
          });

          ViewStyleLegend.belongsTo(models.ViewStyleType, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'type_id',
              allowNull: false
            }
          });

          ViewStyleLegend.hasMany(models.ViewStyleColor, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "view_style_id",
              allowNull: false
            }
          });
        }
      }
    });

    return ViewStyleLegend;
  };
}());