module.exports = function(sequelize, DataTypes) {
  var Collector = sequelize.define("Collector",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      active: DataTypes.BOOLEAN,
      // todo: check it
      collector_type: DataTypes.INTEGER,
      schedule_type: DataTypes.INTEGER,
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        registerHooks: (models) => {
          /**
           * Adds a project identifier to the Collector instance object
           *
           * Executes after ORM Sequelize performs Query Operation
           * @param {Collector|Collector[]} result Result set
           */
          function afterFind(result/*, options*/) {
            if (result instanceof Array) {
              result.map(collector => collector.project_id = collector.dataValues.project_id = collector.DataSery.project_id);
            } else {
              if (result !== null)
                result.project_id = result.dataValues.project_id = result.DataSery.project_id;
            }
          }

          /**
           * Adds a include model in query filter to retrieve DataSeries object.
           * Executes before Sequelize performs Query Operation.
           *
           * @param {*} options A object containing query filter parameters
           */
          function beforeFind(options) {
            let includeOptions = options.include || [];

            for(let modelObject of includeOptions) {
              // When a data series model is already included, just return query set
              if (modelObject.model.$schema === models.DataSeries.$schema &&
                  modelObject.model.tableName === models.DataSeries.tableName) {
                return;
              }
            }

            // Include the model DataSeries to retrieve DataSeries Object
            includeOptions.push({ model: models.DataSeries });

            // Set include options to original object sequelize filter
            options.include = includeOptions;
          }

          Collector.hook("beforeFind", beforeFind);
          Collector.hook("afterFind", afterFind);
        },

        associate: function(models) {
          Collector.hasOne(models.Filter, {
            foreignKey: {
              allowNull: false,
              unique: true
            }
          });

          Collector.hasMany(models.CollectorInputOutput, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          Collector.hasMany(models.Intersection, {
            onDelete: 'CASCADE',
            foreignKey: {
              name: 'collector_id',
              allowNull: false
            }
          })

          Collector.belongsTo(models.ServiceInstance, {
            foreignKey: {
              allowNull: true
            }
          });

          Collector.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_input",
              allowNull: false
            }
          });

          Collector.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_output",
              allowNull: false
            }
          });

          Collector.belongsTo(models.Schedule, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: true
            }
          });
        }
      }
    }
  );

  return Collector;
};
