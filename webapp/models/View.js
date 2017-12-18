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
        comment: "View description"
      },
      active: {
        type: DataTypes.BOOLEAN,
        allowNull: false,
        default: true,
        comment: "It defines view can be used and retrieved. Default is true."
      },
      private: {
        type: DataTypes.BOOLEAN,
        allowNull: false,
        default: false,
        comment: "It defines if the view is private. Default is false."
      },
      schedule_type: {
        type: DataTypes.INTEGER,
        allowNull: true
      },
      source_type: {
        type: DataTypes.INTEGER,
        allowNull: false,
        comment: "It defines the type of data source that create the view. Alert, Analysis, Static Data or Dynamic Data"
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

          View.belongsTo(models.Schedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "schedule_id",
              allowNull: true,
              constraints: true
            }
          });

          View.belongsTo(models.AutomaticSchedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "automatic_schedule_id",
              allowNull: true,
              constraints: true
            }
          });

          View.hasOne(models.RegisteredView, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "view_id",
              allowNull: false
            }
          });

          View.hasOne(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "view_id",
              allowNull: true
            }
          });

          View.hasOne(models.ViewStyleLegend, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "view_id",
              allowNull: false
            }
          });

          View.hasMany(models.AlertAttachedView, {
            onDelete: 'CASCADE',
            foreignKey: {
              allowNull: false
            }
          });

          View.hasMany(models.ViewProperties, {
            onDelete: 'CASCADE',
            foreignKey: {
              name: "view_id",
              allowNull: false
            }
          });

          View.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });

          View.belongsTo(models.ServiceInstance, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "service_instance_id",
              allowNull: false
            }
          });
        },

        /**
         * It generates a Salt to encrypt/decrypt URI credentials
         * @todo Implement it
         * @returns {string}
         */
        generateSalt: function() {
          return Utils.generateSalt(10);
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