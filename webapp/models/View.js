(function() {
  'use strict';

  /**
   * TerraMA² Utils
   * @type {Utils}
   */
  var Utils = require("./../core/Utils");

  /**
   * The View model is injected as dependency of Sequelize.
   * 
   * @param {Sequelize} sequelize - Sequelize instance
   * @param {Sequelize.DataTypes} DataTypes - Sequelize Database DataTypes
   * @returns {Sequelize.Model}
   */
  module.exports = function(sequelize, DataTypes) {
    /**
     * It defines a Sequelize View model
     * 
     * @type {Sequelize.Model}
     */
    var View = sequelize.define("View", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "View identifier"
      },
      name: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "View name"
      },
      description: {
        type: DataTypes.TEXT,
        allowNull: false,
        comment: "View description"
      }, 
      script: {
        type: DataTypes.TEXT,
        allowNull: false,
        comment: "XML style script"
      },
      uri: {
        type: DataTypes.STRING,
        allowNull: false,
        comment: "Map server connection URI"
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
          View.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_id",
              allowNull: false
            }
          });
        },

        /**
         * It generates a Salt to encrypt/decrypt URI credentials
         * 
         * @returns {string}
         */
        generateSalt: function() {
          return bcrypt.genSaltSync(10);
        },
        /**
         * It generates a crypted URI
         * 
         * @throws {Error} It may throws Error when a uri is not a string or UriSyntax is invalid
         * @param {string} uri - An URI to encrypt
         * @param {Object?} syntax - An URI syntax. Default is Enums.Uri
         * @param {string} salt - A salt to create encrypted URI
         * @returns {string}
         */
        encryptURI: function(uri, syntax, salt) {
          return Utils.encryptURI(uri, syntax, salt);
        },
        /**
         * It compares URI encrypted with another URI
         * 
         * @param {string} uri - An URI to encrypt
         * @param {string} encryptedURI - An encrypted URI
         * @returns {Boolean}
         */
        compareURI: function(uri, encryptedURI) {
          return Utils.compareURI(uri, encryptedURI);
        }
      }
    });

    return View;
  };
}());