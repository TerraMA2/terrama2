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
    var Charts = sequelize.define("Charts", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "Chart identifier"
      },
      name: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "Chart name"
      },
      unit: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "Unit"
      },
      title: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "Chart title"
      },
      type: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "Chart type"
      },
      description: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "Chart description"
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
          Charts.belongsTo(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "view_id",
              allowNull: false
            }
          });
        }
      }
    });

    return Charts;
  };
}());