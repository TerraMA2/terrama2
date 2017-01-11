(function() {
  "use strict";

  /**
   * TerraMA² Utils
   * @type {Utils}
   */
  var Utils = require("./../core/Utils");

  /**
   * It defines a ViewStyleColor table that represents legend for View
   * 
   * @param {Sequelize} sequelize - Sequelize instance
   * @param {Sequelize.DataTypes} DataTypes - Sequelize Database DataTypes
   * @returns {Sequelize.Model}
   */
  module.exports = function(sequelize, DataTypes) {
    /**
     * It defines a Sequelize View Metadata model
     * 
     * @type {Sequelize.Model}
     */
    var ViewStyleColor = sequelize.define("ViewStyleColor", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "ViewStyleColor identifier"
      },
      title: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "Title"
      },
      color: {
        type: DataTypes.STRING,
        comment: "Style Color Hex"
      },
      isDefault: {
        type: DataTypes.BOOLEAN,
        allowNull: false,
        default: false
      }
    }, {
      comment: "It represents the colors for legend of View",
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      classMethods: {
        /**
         * It applies a table association during models loading.
         */
        associate: function(models) {
          ViewStyleColor.belongsTo(models.ViewStyleLegend, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "view_style_id",
              allowNull: false
            }
          });
        }
      }
    });

    return ViewStyleColor;
  };
}());