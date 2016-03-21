'use strict';

/**
 * @license
 * Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.
 *
 * This file is part of TerraMA2 - a free and open source computational
 * platform for analysis, monitoring, and alert of geo-environmental extremes.
 *
 * TerraMA2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.

 * TerraMA2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with TerraMA2. See LICENSE. If not, write to
 * TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

var modelsFn = require("../models");
var exceptions = require('./Exceptions');
var Promise = require('bluebird');
var Utils = require('./Utils');
var _ = require('lodash');

// Javascript Lock
var ReadWriteLock = require('rwlock');
var lock = new ReadWriteLock();


// Helpers
function getItemByParam(array, object) {
  var key = Object.keys(object)[0];
  return array.find(function(item){
    return (item[key] == object[key]);
  });
}


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
  isLoaded: false,

  /**
   * It sets active database configuration for models synchronization
   * @param {Object} configuration - An object with database configuration
   */
  setConfiguration: function(configuration) {
    if (configuration && !_.isEqual(actualConfig, configuration))
      actualConfig = configuration;
  },

  /**
   * It initializes DataManager, loading models and database synchronization
   * @param {function} callback - A callback function for waiting async operation
   */
  init: function(callback) {
    var self = this;

    // Lock function
    lock.readLock(function (release) {
      var Sequelize = require("sequelize");

      if (actualConfig) {
        var connection = new Sequelize(actualConfig.database,
          actualConfig.username,
          actualConfig.password,
          actualConfig);

        models = modelsFn();
        models.load(connection);

        self.connection = connection;

        var releaseCallback = function() {
          release();
          callback();
        };

        var fn = function() {
          // todo: insert default values in database
          self.addDataProviderType({name: "FILE", description: "Desc File"}).then(function() {
            self.addDataProviderType({name: "FTP", description: "Desc Type1"}).then(function() {
              self.addDataProviderType({name: "HTTP", description: "Desc Http"}).then(function() {
                self.addDataProviderType({name: "POSTGIS", description: "Desc Postgis"}).then(function() {
                  models.db.DataProviderIntent.create({name: "Intent1", description: "Desc Intent2"}).then(function(){
                    self.addDataFormat({name: "Format 1", description: "Format Description"}).then(function() {
                      models.db.DataSeriesType.create({name: "DS Type 1", description: "DS Type1 Desc"}).then(function() {
                        releaseCallback();
                      }).catch(function() {
                        releaseCallback();
                      })
                    }).catch(function() {
                      releaseCallback();
                    });
                  }).catch(function(){
                    releaseCallback();
                  });
                })
              }).catch(function() {
                releaseCallback();
              })
            }).catch(function(){
              releaseCallback();
            });
          }).catch(function() {
            releaseCallback();
          })
        };

        connection.sync().then(function () {
          fn();
        }, function(err) {
          fn();
        });
      }
      else {
        releaseCallback();
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
      if (self.isLoaded) {
        resolve();
        return;
      }

      var clean = function() {
        self.data.dataProviders = [];
        self.data.dataSeries = [];
        self.data.dataSets = [];
        self.data.projects = [];
      };

      models.db.Project.findAll({}).then(function(projects) {
        projects.forEach(function(project) {
          self.data.projects.push(project.get());
        });

        models.db.DataProvider.findAll({}).then(function(dataProviders){
          dataProviders.forEach(function(dataProvider) {
            self.data.dataProviders.push(dataProvider.get());
          });

          models.db.DataSeries.findAll({}).then(function(dataSeries) {
            dataSeries.forEach(function(dSeries) {
              self.data.dataSeries.push(dSeries.get());
            });
            //self.data.dataSeries = dataSeries;
            self.isLoaded = true;
            resolve();
          }).catch(function(err) {
            clean();
            reject(err);
          });

        }).catch(function(err) {
          clean();
          reject(err);
        });
      }).catch(function(err) {
        clean();
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

      lock.writeLock(function(release) {
        models.db.Project.create(projectObject).then(function(project){
          self.data.projects.push(project.get());
          resolve(Utils.clone(project.get()));
          release();
        }).catch(function(e) {
          reject(e);
          release();
        });
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

      lock.readLock(function(release) {
        var project = getItemByParam(self.data.projects, projectParam);
        if (project)
          resolve(Utils.clone(project));
        else
          reject(new exceptions.ProjectError("Project not found"));

        release();
      });
    });
  },

  updateProject: function(projectObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getProject({id: projectObject.id}).then(function(project) {

        models.db.Project.update(projectObject, {
          fields: ["name", "description", "version"],
          where: {
            id: project.id
          }
        }).then(function(rows) {
          var projectItem = getItemByParam(self.data.projects, {id: projectObject.id});
          projectItem.name = projectObject.name;
          projectItem.description = projectObject.description;
          projectItem.version = projectObject.version;

          resolve(Utils.clone(projectItem));
        }).catch(function(err) {
          reject(new exceptions.ProjectError("Could update project"));
        });
      }).catch(function(err) {
        reject(err);
      });
    });
  },

  listProjects: function() {
    var projectList = [];
    for(var index = 0; index < this.data.projects.length; ++index)
      projectList.push(Utils.clone(this.data.projects[index]));
    return projectList;
  },

  /**
   * It saves DataProviderType in database.
   * @todo Load it in memory
   *
   * @param {Object} dataProviderTypeObject - An object containing needed values to create DataProviderType object.
   * @return {Promise} - a 'bluebird' module with semantics instance or error callback.
   */
  addDataProviderType: function(dataProviderTypeObject) {
    return new Promise(function(resolve, reject) {
      models.db.DataProviderType.create(dataProviderTypeObject).then(function(result) {
        resolve(Utils.clone(result.get()));
      }).catch(function(err) {
        reject(err);
      })
    });
  },

  listDataProviderType: function() {
    return new Promise(function(resolve, reject) {
      models.db.DataProviderType.findAll({}).then(function(result) {
        var output = [];
        result.forEach(function(element) {
          output.push(Utils.clone(element.get()));
        });

        resolve(output);
      }).catch(function(err) {
        reject(err);
      })
    });
  },

  /**
   * It saves DataFormat in database.
   *
   * @param {Object} dataFormatObject - An object containing needed values to create DataFormatObject object.
   * @return {Promise} - a 'bluebird' module with semantics instance or error callback.
   */
  addDataFormat: function(dataFormatObject) {
    return new Promise(function(resolve, reject) {
      models.db.DataFormat.create(dataFormatObject).then(function(result) {
        resolve(Utils.clone(result.get()));
      }).catch(function(err) {
        reject(err);
      })
    });
  },

  /**
   * It saves DataSeriesSemantics in database.
   *
   * @param {Object} semanticsObject - An object containing needed values to create DataSeriesSemantics object.
   * @return {Promise} - a 'bluebird' module with semantics instance or error callback.
   */
  addDataSeriesSemantics: function(semanticsObject) {
    return new Promise(function(resolve, reject){
      models.db.DataSeriesSemantics.create(semanticsObject).then(function(semantics){
        resolve(Utils.clone(semantics.get()));
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
        self.data.dataProviders.push(dataProvider.get());
        resolve(Utils.clone(dataProvider.get()));

        //  todo: emit signal

      }).catch(function(err){
        reject(new exceptions.DataProviderError("Could not save data provider. " + err.message));
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
      var dataProvider = getItemByParam(self.data.dataProviders, restriction);
      if (dataProvider){
        resolve(Utils.clone(dataProvider));
      }
      else
        reject(new exceptions.DataProviderError("Could not find a data provider: " + restriction[Object.keys(restriction)[0]]));
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
      dataProviderObjectList.push(Utils.clone(this.data.dataProviders[index]));
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
      var dataProvider = getItemByParam(self.data.dataProviders, dataProviderObject);

      if (dataProvider) {
        models.db.DataProvider.update(dataProviderObject, {
          fields: ["name", "description", "uri", "active"],
          where: {
            id: dataProvider.id
          }
        }).then(function() {
          if (dataProviderObject.name)
            dataProvider.name = dataProviderObject.name;

          if (dataProviderObject.description)
            dataProvider.description = dataProviderObject.description;

          if (dataProviderObject.uri)
            dataProvider.uri = dataProviderObject.uri;

          dataProvider.active = dataProviderObject.active;

          resolve(Utils.clone(dataProvider));
        }).catch(function(err) {
          reject(new exceptions.DataProviderError("Could not update data provider ", err));
        });
      } else
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
          models.db.DataProvider.destroy({where: {id: provider.id}}).then(function() {
            self.data.dataProviders.splice(index, 1);
            resolve();
          }).catch(function(err) {
            reject(err);
          });
          return;
        }
      }
      reject(new exceptions.DataManagerError("DataProvider not found"));
    });
  },

  /**
   * It retrieves a DataSeries object from restriction. It should be an object containing either id identifier or
   * name identifier.
   *
   * @param {Object} restriction - An object containing DataSeries identifier to get it.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  getDataSerie: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataSerie = getItemByParam(self.data.dataSeries, restriction);
      if (dataSerie)
        resolve(Utils.clone(dataSerie));
      else
        reject(new exceptions.DataSeriesError("Could not find a data series: ", restriction));
    });
  },

  /**
   * It retrieves DataSeries loaded in memory.
   *
   * @return {Array<DataSeries>} - An array with DataSeries available/loaded in memory.
   */
  listDataSeries: function() {
    var dataSeriesList = [];
    for(var index = 0; index < this.data.dataSeries.length; ++index)
      dataSeriesList.push(Utils.clone(this.data.dataSeries[index]));
    return dataSeriesList;
  },

  /**
   * It saves a DataSeries object in database. It also saves DataSet if there are. The object syntax is:
   * {
   *   modelValuesObject...
   *
   *   dataSets: [
   *     {
   *       type: DataSet Child type (dcp, occurrence)
   *       dataSetValuesObject...
   *       child: {
   *         dataSetChildValues...
   *       }
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
      var output;
      models.db.DataSeries.create(dataSeriesObject).then(function(dataSerie){
        //self.data.dataSeries.push(dataSerie.get());
        output = Utils.clone(dataSerie.get());
        // if there DataSets to save too
        if (dataSeriesObject.dataSets) {
          var dataSets = [];
          for(var i = 0; i < dataSeriesObject.dataSets.length; ++i) {
            var dSet = dataSeriesObject.dataSets[i];
            dataSets.push(self.addDataSet(dSet.type, dSet));
          }

          return Promise.all(dataSets);

        } else {
          // rollback
          dataSerie.destroy().then(function () {
            reject(new exceptions.DataSeriesError("Could not save DataSeries. Data sets not found" + err));
          });
        }
      }).then(function(dataSets){
        self.data.dataSeries.push(Utils.clone(output));
        output.dataSets = dataSets;
        resolve(output);
      }).catch(function(err){
        reject(new exceptions.DataSeriesError("Could not save DataSeries. " + err));
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
      var dataSeries = getItemByParam(self.data.dataSeries, {id: dataSeriesObject.id});

      if (dataSeries) {
        models.db.DataSeries.update(dataSeriesObject, {
          fields: ['name', 'description'],
          where: {
            $or: [
              {id: dataSeriesObject.id},
              {name: dataSeriesObject.name}
            ]
          }
        }).then(function(rows) {
          dataSeries.name = dataSeriesObject.name;
          dataSeries.description = dataSeriesObject.description;

          resolve(Utils.clone(dataSeries));
        }).catch(function(err) {
          reject(new exceptions.DataSeriesError("Could not update data series ", err));
        });

      } else
        reject(new exceptions.DataSeriesError("Data series not found. "));
    });
  },

  /**
   * It removes a DataSeries object. It should be an object containing object filled out with identifier
   * and model values.
   *
   * @param {Object} dataSeriesParam - An object containing DataSeries identifier to remove it.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  removeDataSerie: function(dataSeriesParam) {
    var self = this;
    return new Promise(function(resolve, reject) {
      for(var index = 0; index < self.data.dataSeries.length; ++index) {
        var dataSeries = self.data.dataSeries[index];
        if (dataSeries.id === dataSeriesParam.id || dataSeries.name === dataSeriesParam.name) {
          models.db.DataSeries.destroy({where: {
            $or: [
              {id: dataSeriesParam.id},
              {name: dataSeriesParam.name}
            ]
          }}).then(function (status) {
            self.data.dataSets.forEach(function(dSet, dSetIndex, array) {
              if (dSet.data_series_id === dataSeries.id)
                self.data.dataSets.splice(dSetIndex, 1);
            });
            self.data.dataSeries.splice(index, 1);
            resolve(status);
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
   * @example
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
   * @param {Array<Object>} dataSetObject - An object containing DataSet values to save it.
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

        // rollback data set function if any error occurred
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

  /**
   * It retrieves a DataSet object. It should be an object containing object filled out with identifier
   * and model values.
   *
   * @param {Object} restriction - An object containing DataSet identifier to get it and type of DataSet.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  getDataSet: function(restriction) {
    /**
     * { id / data_series_id: someValue, type: SomeType(dcp, occurrence)
     */
    var self = this;
    return new Promise(function(resolve, reject) {
      lock.readLock(function (release) {
        var dataSet = getItemByParam(self.data.dataSets, restriction);
        if (dataSet)
          resolve(Utils.clone(dataSet));
        else
          reject(new exceptions.DataSetError("Could not find a data set: ", restriction));
        release();
      });
    });
  },

  /**
   * It updates a DataSet object. It should be an object containing object filled out with identifier
   * and model values.
   *
   * @param {Object} restriction - An object containing DataSeries identifier to get it.
   * @param {Object} dataSetObject - An object containing DataSet values to be updated.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  updateDataSet: function(restriction, dataSetObject) {
    /**
     * restriction => {
     *   id: someValue,
     *   type: SomeType(dcp, occurrence),
     * }
     *
     * dataSetObject => {
     *   active: ...,
     *   timeColumn: ...,
     *   geometryColumn: ...
     *   ...
     * }
     */

    var self = this;
    return new Promise(function(resolve, reject) {

      var dataSet = getItemByParam(self.data.dataSets, restriction);

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
      dataSetsList.push(Utils.clone(this.data.dataSets[index]));

    return dataSetsList;
  }

};

module.exports = DataManager;