(function() {
  "use strict";

  /**
   * It represents chart for View
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
    var ChartsAttributes = sequelize.define("ChartAttributes", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "Chart identifier"
      },
      attribute_name: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "Attribute"
      },
    }, {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      classMethods: {
        /**
         * It applies a table association during models loading.
         */
        associate: function(models) {
          ChartsAttributes.belongsTo(models.Charts, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "chart_id",
              allowNull: false
            }
          });
        }
      }
    });

    return ChartsAttributes;
  };
}());