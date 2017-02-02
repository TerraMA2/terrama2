(function() {
  "use strict";

  /**
   * It represents legend  metadata for View representaiton
   * 
   * @param {Sequelize} sequelize - Sequelize instance
   * @param {Sequelize.DataTypes} DataTypes - Sequelize Database DataTypes
   * @returns {Sequelize.Model}
   */
  module.exports = function(sequelize, DataTypes) {
    var ViewStyleLegendMetadata = sequelize.define("ViewStyleLegendMetadata", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "ViewStyleLegendMetadata identifier"
      },
      key: DataTypes.STRING,
      value: DataTypes.STRING
    }, {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      classMethods: {
        /**
         * It applies a table association during models loading.
         */
        associate: function(models) {
          ViewStyleLegendMetadata.belongsTo(models.ViewStyleLegend, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'legend_id',
              allowNull: false
            }
          });
        }
      }
    });

    return ViewStyleLegendMetadata;
  };
}());