(function() {
  "use strict";

  /**
   * It represents interpolator metadata for interpolator representation
   * 
   * @param {Sequelize} sequelize - Sequelize instance
   * @param {Sequelize.DataTypes} DataTypes - Sequelize Database DataTypes
   * @returns {Sequelize.Model}
   */
  module.exports = function(sequelize, DataTypes) {
    var InterpolatorMetadata = sequelize.define("InterpolatorMetadata", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "InterpolatorMetadata identifier"
      },
      key: DataTypes.STRING,
      value: DataTypes.TEXT
    }, {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      classMethods: {
        /**
         * It applies a table association during models loading.
         */
        associate: function(models) {
          InterpolatorMetadata.belongsTo(models.Interpolator, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'interpolator_id',
              allowNull: false
            }
          });
        }
      }
    });

    return InterpolatorMetadata;
  };
}());