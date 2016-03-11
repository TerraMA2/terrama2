'use strict';

var modelsFn = require("../models");
var exceptions = require('./Exceptions');
var Promise = require('bluebird');
var Utils = require('./Utils');
var _ = require('lodash');
var app = require('../app');
var tr = app.i18n;

// Javascript Lock
var ReadWriteLock = require('rwlock');
var lock = new ReadWriteLock();



// Helpers
Array.prototype.getItemByParam = function(object) {
  var key = Object.keys(object)[0];
  return this.find(function(item){
    return (item[key] == object[key]);
  });
};

var models = null;
var actualConfig = {};

/**
 * Controller of the system index.
 * @class DataManager
 *
 * @property {object} data - Object for storing model values, such DataProviders, DataSeries and Projects.
 * @property {object} connection - 'sequelize' module connection.
 */
var DataManager = {
  data: {
    dataFormats: [],
    dataSeriesTypes: [],
    dataSeriesSemantics: [],

    dataSeries: [],
    dataProviders: [],
    dataSets: [], /** It will store data set id list */
    projects: []
  },
  connection: null,

  /**
   * It initializes DataManager, loading models and database synchronization
   * @param {function} callback - A callback function for waiting async operation
   */
  init: function(callback) {
    var self = this;

    // Lock function
    lock.readLock(function (release) {
      var Sequelize = require("sequelize");
      var databaseConfig = app.get("databaseConfig");

      if (!_.isEqual(databaseConfig, actualConfig)) {
        actualConfig = databaseConfig;

        var connection = new Sequelize(actualConfig.database,
          actualConfig.username,
          actualConfig.password,
          actualConfig);

        models = modelsFn();
        models.load(connection);

        self.connection = connection;


        var fn = function() {
          // todo: insert default values in database
          self.addDataProviderType({name: "FTP", description: "Desc Type1"}).then(function(providerType) {
            models.db.DataProviderIntent.create({name: "Intent1", description: "Desc Intent2"}).then(function(intent){
              self.addDataFormat({name: "Format 1", description: "Format Description"}).then(function(format) {
                models.db.DataSeriesType.create({name: "DS Type 1", description: "DS Type1 Desc"}).then(function(dsType) {
                  release();
                  callback();
                }).catch(function(err) {
                  release();
                  callback();
                })
              }).catch(function(err) {
                release();
                callback();
              });
            }).catch(function(e){
              release();
              callback();
            });
          }).catch(function(e){
            release();
            callback();
          });
        };

        connection.sync().then(function () {
          fn();
        }, function(err) {
          fn();
        });
      }
      else {
        release();
        callback();
      }
    });
  },

  /**
   * It loads database values in memory
   * @return {Promise} - a 'bluebird' module
   */
  load: function() {
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.Project.findAll({}).then(function(projects) {
        self.data.projects = projects;

        models.db.DataProvider.findAll({}).then(function(dataProviders){
          self.data.dataProviders = dataProviders;

          models.db.DataSeries.findAll({}).then(function(dataSeries) {
            self.data.dataSeries = dataSeries;
            resolve();
          }).catch(function(err) {
            reject(err);
          });

        }).catch(function(err) {
          reject(err);
        });
      }).catch(function(err) {
        reject(err);
      });
    });
  },

  /**
   * It saves Project in database and storage it in memory
   * @param {Object} projectObject - An object containing project values to be saved.
   * @return {Promise} - a 'bluebird' module. The callback is either a {Project} data values or error
   */
  addProject: function(projectObject) {
    var self = this;
    return new Promise(function(resolve, reject){
      models.db.Project.create(projectObject).then(function(project){
        self.data.projects.push(project);
        resolve(Utils.clone(project.dataValues));
      }).catch(function(e) {
        reject(e);
      });
    });
  },

  /**
   * It retrieves a project from restriction. It should be an object containing either id identifier or name identifier.
   * @param {Object} projectParam - An object containing project identifier. i.e {name: "Project1"}
   * @return {Promise} - a 'bluebird' module. The callback is a clone of project or error
   */
  getProject: function(projectParam) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var project = self.data.projects.getItemByParam(projectParam);
      if (project)
        resolve(Utils.clone(project.dataValues));
      else
        reject(new exceptions.ProjectError("Project not found"));
    });
  },

  /**
   * It saves DataProviderType in database.
   * @todo Load it in memory
   *
   * @param {Object} dataProviderTypeObject - An object containing needed values to create DataProviderType object.
   * @return {Promise} - a 'bluebird' module with semantic instance or error callback.
   */
  addDataProviderType: function(dataProviderTypeObject) {
    return new Promise(function(resolve, reject) {
      models.db.DataProviderType.create(dataProviderTypeObject).then(function(result) {
        resolve(Utils.clone(result.dataValues));
      }).catch(function(err) {
        reject(err);
      })
    });
  },

  /**
   * It saves DataFormat in database.
   *
   * @param {Object} dataFormatObject - An object containing needed values to create DataFormatObject object.
   * @return {Promise} - a 'bluebird' module with semantic instance or error callback.
   */
  addDataFormat: function(dataFormatObject) {
    return new Promise(function(resolve, reject) {
      models.db.DataFormat.create(dataFormatObject).then(function(result) {
        resolve(Utils.clone(result.dataValues));
      }).catch(function(err) {
        reject(err);
      })
    });
  },

  /**
   * It saves DataSeriesSemantic in database.
   *
   * @param {Object} semanticObject - An object containing needed values to create DataSeriesSemantic object.
   * @return {Promise} - a 'bluebird' module with semantic instance or error callback.
   */
  addDataSeriesSemantic: function(semanticObject) {
    return new Promise(function(resolve, reject){
      models.db.DataSeriesSemantic.create(semanticObject).then(function(semantic){
        resolve(Utils.clone(semantic));
      }).catch(function(e) {
        reject(e);
      });
    });
  },

  /**
   * It saves DataProvider in database and load it in memory
   * @param {Object} dataProviderObject - An object containing needed values to create DataProvider object.
   * @return {Promise} - a 'bluebird' module with DataProvider instance or error callback
   */
  addDataProvider: function(dataProviderObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataProvider.create(dataProviderObject).then(function(dataProvider){
        self.data.dataProviders.push(dataProvider);
        resolve(Utils.clone(dataProvider.dataValues));

        //  todo: emit signal

      }).catch(function(err){
        var error = exceptions.DataProviderError("Could not save data provider. " + err);
        reject(error);
      });
    });
  },

  /**
   * It retrieves a DataProvider object from restriction. It should be an object containing either id identifier or
   * name identifier.
   *
   * @param {Object} restriction - An object containing DataProvider identifier to get it.
   * @return {Promise} - a 'bluebird' module with DataProvider instance or error callback
   */
  getDataProvider: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataProvider = self.data.dataProviders.getItemByParam(restriction);
      if (dataProvider){
        resolve(Utils.clone(dataProvider.dataValues));
      }
      else
        reject(new exceptions.DataProviderError("Could not find a data provider: ", restriction));
    });
  },

  /**
   * It retrieves DataProviders loaded in memory.
   *
   * @return {Array} - An array with DataProviders available/loaded in memory.
   */
  listDataProviders: function() {
    var dataProviderObjectList = [];
    for(var index = 0; index < this.data.dataProviders.length; ++index)
      dataProviderObjectList.push(Utils.clone(this.data.dataProviders[index].dataValues));
    return dataProviderObjectList;
  },

  /**
   * It updates a DataProvider instance from object.
   *
   * @param {Object} dataProviderObject - An object containing DataProvider identifier to get it.
   * @return {Promise} - a 'bluebird' module with DataProvider instance or error callback
   */
  updateDataProvider: function(dataProviderObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      for(var i = 0; i < self.data.dataProviders.length; ++i) {
        var provider = self.data.dataProviders[i];
        if (provider.id === dataProviderObject.id) {
          provider.updateAttributes({
            name: dataProviderObject.name,
            description: dataProviderObject.description,
            uri: dataProviderObject.uri,
            active: dataProviderObject.active
          }).then(function() {
            self.data.dataProviders[i] = provider;
            resolve(Utils.clone(provider.dataValues));
          }).catch(function(err) {
            reject(new exceptions.DataProviderError("Could not update data provider ", err));
          });
          return;
        }
      }

      reject(new exceptions.DataProviderError("Data provider not found"));
    });
  },

  /**
   * It removes DataProvider from param. It should be an object containing either id identifier or
   * name identifier.
   *
   * @param {Object} dataProviderParam - An object containing DataProvider identifier to get it.
   * @return {Promise} - a 'bluebird' module with DataProvider instance or error callback
   */
  removeDataProvider: function(dataProviderParam) {
    var self = this;
    return new Promise(function(resolve, reject) {
      for(var index = 0; index < self.data.dataProviders.length; ++index) {
        var provider = self.data.dataProviders[index];
        if (provider.id === dataProviderParam.id || provider.name === dataProviderParam.name) {
          provider.destroy().then(function (status) {
            resolve(status);
            self.data.dataProviders.splice(index, 1);
          }).catch(function (err) {
            reject(err);
          });
          return;
        }
      }
      reject(new exceptions.DataManagerError("DataManager not initialized yet"));
    });
  },

  /**
   * It retrieves a DataSeries object from restriction. It should be an object containing either id identifier or
   * name identifier.
   *
   * @param {Object} restriction - An object containing DataSeries identifier to get it.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  getDataSerie: function(restriction)
  {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataSerie = self.data.dataSeries.getItemByParam(restriction);
      if (dataSerie)
        resolve(Utils.clone(dataSerie.dataValues));
      else
        reject(new exceptions.DataSeriesError("Could not find a data series: ", restriction));
    });
  },

  listDataSeries: function() {
    var dataSeriesList = [];
    for(var index = 0; index < this.data.dataSeries.length; ++index)
      dataSeriesList.push(Utils.clone(this.data.dataSeries[index].dataValues));
    return dataSeriesList;
  },

  /**
   * It saves a DataSeries object in database. It also saves DataSet if there are. The object syntax is:
   * {
   *   modelValuesObject...
   *
   *   dataSets: [
   *     {
   *       dataSetValuesObject...
   *     }
   *   ]
   * }
   *
   * @param {Object} dataSeriesObject - An object containing DataSeries values to save it.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  addDataSerie: function(dataSeriesObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataSeries.create(dataSeriesObject).then(function(dataSerie){
        // if there DataSets to save too
        if (dataSeriesObject.dataSets) {
          models.db.DataSet.bulkCreate(dataSeriesObject.dataSets).then(function () {
            models.db.DataSet.findAll({data_series_id: dataSerie.id}).then(function (dSets) {
              dataSerie.setDataSets(dSets).then(function (result) {
                self.data.dataSeries.push(dataSerie);
                resolve(Utils.clone(dataSerie.dataValues));
              }).catch(function (err) {
                reject(err);
              });
            }).catch(function (err) {
              reject(err);
            });
          }).catch(function (err) {
            reject(err);
          });
        } else {
          self.data.dataSeries.push(dataSerie);
          resolve(Utils.clone(dataSerie.dataValues));
        }
      }).catch(function(err){
       reject(new exceptions.DataProviderError("Could not save DataSeries. " + err));
      });
    });
  },

  /**
   * It updates a DataSeries object. It should be an object containing object filled out with identifier
   * and model values.
   *
   * @param {Object} dataSeriesObject - An object containing DataSeries identifier to get it.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  updateDataSerie: function(dataSeriesObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      for(var i = 0; i < self.data.dataSeries.length; ++i) {
        var element = self.data.dataSeries[i];

        if (element.id === dataSeriesObject.id) {
          element.updateAttributes({
            name: dataSeriesObject.name,
            description: dataSeriesObject.description
          }).then(function() {
            resolve(Utils.clone(element.dataValues));
          }).catch(function(err) {
            reject(new exceptions.DataSeriesError("Could not update data series ", err));
          });
          return;
        }
      }

      reject(new exceptions.DataSeriesError("Data series not found. "));
    });
  },

  removeDataSerie: function(dataSeriesParam) {
    var self = this;
    return new Promise(function(resolve, reject) {
      for(var index = 0; index < self.data.dataSeries.length; ++index) {
        var dataSeries = self.data.dataSeries[index];
        if (dataSeries.id === dataSeriesParam.id || dataSeries.name === dataSeriesParam.name) {
          dataSeries.destroy().then(function (status) {
            resolve(status);
            self.data.dataSeries.splice(index, 1);
          }).catch(function (err) {
            reject(err);
          });
          return;
        }
      }

      reject(new exceptions.DataSeriesError("Data series not found"));
    });
  },

  /**
   * It saves a DataSet object in database. The object syntax is:
   * {
   *   active: true,
   *   data_series_id: someID,
   *   child: -> {
   *     timeColumn...
   *     geometryColumn...
   *   }
   *   dataFormats: [{},{},{}]
   * }
   *
   * @param {string} dataSetType - A string value representing DataSet type. (dcp, occurrence, grid).
   * @param {Object} dataSetObject - An object containing DataSet values to save it.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  addDataSet: function(dataSetType, dataSetObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataSet.create({
        active: dataSetObject.active,
        data_series_id: dataSetObject.data_series_id
      }).then(function(dataSet) {

        var onSuccess = function(dSet) {
          var output = Utils.clone(dataSet.get());
          output.dataSetType = dataSetType;
          output.child = Utils.clone(dSet.get());

          // save dataformat
          if (dataSetObject.dataFormats) {
            for(var i = 0; i < dataSetObject.dataFormats.length; ++i) {
              dataSetObject.dataFormats[i].data_set_id = dataSet.id;
            }

            models.db.DataSetFormat.bulkCreate(dataSetObject.dataFormats, {data_set_id: dataSet.id}).then(function () {
              models.db.DataSetFormat.findAll({data_set_id: dataSet.id}).then(function(formats) {
                output.dataFormats = [];
                for(var i = 0; i < formats.length; ++i)
                  output.dataFormats.push(Utils.clone(formats[i].dataValues));

                self.data.dataSets.push(output);

                resolve(output);
              });
            }).catch(function (err) {
              reject(err);
            });
          } else {// todo: validate it
            resolve(output);
            self.data.dataSets.push(output);
          }

        };

        var onError = function(err) {
          reject(new exceptions.DataSetError("Could not save data set." + err));
        };

        var rollback = function(dataSet) {
          dataSet.destroy().then(function() {
            reject(new exceptions.DataSetError("Invalid dataset type. DataSet destroyed"));
          }).catch(onError);
        };

        if (dataSetType && typeof(dataSetType) === 'string') {
          switch(dataSetType) {
            case "dcp":
              models.db.DataSetDcp.create(dataSetObject.child).then(onSuccess).catch(onError);
              break;
            case "occurrence":
              models.db.DataSetOccurrence.create(dataSetObject.child).then(onSuccess).catch(onError);
              break;
            case "grid":
              models.db.DataSetDcp.create(dataSetObject.child).then(onSuccess).catch(onError);
              break;
            default:
              rollback(dataSet);
          }
        } else {
          rollback(dataSet);
        }

      }).catch(function(err) {
        reject(err);
      });
    });
  },

  getDataSet: function(restriction) {
    /**
     * { id / data_series_id: someValue, type: SomeType(dcp, occurrence)
     */
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataSet = self.data.dataSets.getItemByParam(restriction);
      if (dataSet) {
        resolve(Utils.clone(dataSet));
        //if (restriction.type) {
        //  dataSet.getDataSet(restriction.type).then(function(dset) {
        //    var output = Utils.clone(dset.dataValues);
        //    output.active = dataSet.active;
        //    output.data_series_id = dataSet.data_series_id;
        //    resolve(output);
        //  }).catch(function(err) {
        //    reject(err);
        //  });
        //} else
        //  reject("DataSet type not found")
      }
      else
        reject(new exceptions.DataSetError("Could not find a data set: ", restriction));
    });
  },

  updateDataSet: function(restriction, dataSetObject) {
    /**
     * restriction => {
     *   id: someValue,
     *   type: SomeType(dcp, occurrence),
     * }
     *
     * dataSetObject => {
     *   timeColumn: ...,
     *   geometryColumn: ...
     *   ...
     * }
     */

    var self = this;
    return new Promise(function(resolve, reject) {

      var dataSet = self.data.dataSets.getItemByParam(restriction);

      if (dataSet) {

        models.db.DataSet.find({id: dataSet.id}).then(function(result) {
          result.updateAttributes({active: dataSetObject.active}).then(function() {
            result.getDataSet(restriction.type).then(function(dSet) {
              dSet.updateAttributes(dataSetObject).then(function() {
                var output = Utils.clone(result.get());
                output.child = Utils.clone(dSet.get());
                resolve(output);
              }).catch(function(err) {
                reject(err);
              })
            });
          }).catch(function(err) {
            reject(err);
          });
        }).catch(function(err) {
          reject(err);
        });

      } else
        reject(new exceptions.DataSeriesError("Could not find a data set: ", restriction));
    });
  },

  /**
   * It retrieves a list of DataSets in memory.
   *
   * @param {Object} dataSetId - An object containing Dataset identifier. {id: someValue}
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  removeDataSet: function(dataSetId) {
    var self = this;
    return new Promise(function(resolve, reject) {
      for(var index = 0; index < self.data.dataSets.length; ++index) {
        var dataSet = self.data.dataSets[index];
        if (dataSet.id === dataSetId.id) {
          models.db.DataSet.destroy({where: {id: dataSet.id}}).then(function (status) {
            resolve(status);
            self.data.dataSets.splice(index, 1);
          }).catch(function (err) {
            reject(err);
          });
          return;
        }
      }

      reject(new exceptions.DataSetError("Data set not found."))
    });
  },

  /**
   * It retrieves a list of DataSets in memory.
   *
   * @return {Array<DataSet>} - a 'bluebird' module with DataSeries instance or error callback
   */
  listDataSets: function() {
    var dataSetsList = [];
    for(var index = 0; index < this.data.dataSets.length; ++index)
      dataSetsList.push(Utils.clone(this.data.dataSets[index].dataValues));

    return dataSetsList;
  }

};

module.exports = DataManager;