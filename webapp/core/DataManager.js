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
var Enums = require('./Enums');
var connection = require('../config/Sequelize.js');

// Tcp
var TcpManager = require('./TcpManager');

// data model
var DataProvider = require("./data-model/DataProvider");
var DataSeries = require("./data-model/DataSeries");
var DataSetDcp = require("./data-model/DataSetDcp");
var DataSetFactory = require("./data-model/DataSetFactory");
var Schedule = require('./data-model/Schedule');
var Collector = require('./data-model/Collector');
var Analysis = require('./data-model/Analysis');
var AnalysisDataSeries = require('./data-model/AnalysisDataSeries');


// Available DataSeriesType
var DataSeriesType = Enums.DataSeriesType;

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

/**
 * Controller of the system index.
 * @class DataManager
 *
 * @property {object} data - Object for storing model values, such DataProviders, DataSeries and Projects.
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
  isLoaded: false,

  /**
   * It initializes DataManager, loading models and database synchronization
   * @param {function} callback - A callback function for waiting async operation
   */
  init: function(callback) {
    var self = this;

    // Lock function
    lock.readLock(function (release) {
      var releaseCallback = function() {
        release();
        callback();
      };

      models = modelsFn();
      models.load(connection);

      var fn = function() {
        // todo: insert default values in database
        var inserts = [];

        // default users
        var salt = models.db.User.generateSalt();
        inserts.push(models.db.User.create({
          name: "TerraMA2 User",
          username: "terrama2",
          password: models.db.User.generateHash("terrama2", salt),
          salt: salt,
          cellphone: '14578942362',
          email: 'terrama2@terrama2.inpe.br',
          administrator: false
        }));

        // services type
        inserts.push(models.db.ServiceType.create({name: "COLLECT"}));
        inserts.push(models.db.ServiceType.create({name: "ANALYSIS"}));

        // data provider type defaults
        inserts.push(self.addDataProviderType({name: "FILE", description: "Desc File"}));
        inserts.push(self.addDataProviderType({name: "FTP", description: "Desc Type1"}));
        inserts.push(self.addDataProviderType({name: "HTTP", description: "Desc Http"}));
        inserts.push(self.addDataProviderType({name: "POSTGIS", description: "Desc Postgis"}));

        // data provider intent defaults
        inserts.push(models.db.DataProviderIntent.create({name: "COLLECT", description: "Desc Collect intent"}));
        inserts.push(models.db.DataProviderIntent.create({name: "PROCESSING", description: "Desc Processing intent"}));

        // data series type defaults
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.DCP, description: "Data Series DCP type"}));
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.OCCURRENCE, description: "Data Series Occurrence type"}));
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.GRID, description: "Data Series Grid type"}));

        // data formats semantics defaults todo: check it
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.CSV, description: "DCP description"}));
        inserts.push(self.addDataFormat({name: DataSeriesType.OCCURRENCE, description: "Occurrence description"}));
        inserts.push(self.addDataFormat({name: DataSeriesType.GRID, description: "Grid Description"}));

        // analysis data series type
        inserts.push(models.db["AnalysisDataSeriesType"].create({id: 1, name: "Monitored Object", description: "Description 1"}));

        // semantics
        inserts.push(self.addDataSeriesSemantics({name: "DCP-INPE", data_format_name: Enums.DataSeriesFormat.CSV, data_series_type_name: DataSeriesType.DCP}));
        inserts.push(self.addDataSeriesSemantics({name: "DCP-postgis", data_format_name: Enums.DataSeriesFormat.CSV, data_series_type_name: DataSeriesType.DCP}));
        inserts.push(self.addDataSeriesSemantics({name: "OCCURRENCE-wfp", data_format_name: "Occurrence", data_series_type_name: DataSeriesType.OCCURRENCE}));
        inserts.push(self.addDataSeriesSemantics({name: "OCCURRENCE-postgis", data_format_name: "Occurrence", data_series_type_name: DataSeriesType.OCCURRENCE}));

        Promise.all(inserts).then(function() {
          releaseCallback();
        }).catch(function(err) {
          console.log(err);
          releaseCallback()
        });
      };

      connection.sync().then(function () {
        fn();
      }, function() {
        fn();
      });
    });
  },
  
  unload: function() {
    var self = this;
    return new Promise(function(resolve, reject) {
      lock.writeLock(function(release) {
        self.data.dataProviders = [];
        self.data.dataSeries = [];
        self.data.dataSets = [];
        self.data.projects = [];

        self.isLoaded = false;

        resolve();

        release();
      });
    });
  },
  
  finalize: function() {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.unload().then(function() {
        resolve();

        connection.close();
      });
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
            self.data.dataProviders.push(new DataProvider(dataProvider.get()));
          });

          models.db.DataSeries.findAll({}).then(function(dataSeries) {

            dataSeries.forEach(function(dSeries) {
              self.data.dataSeries.push(new DataSeries(dSeries.get()));

              //todo: include grid too
              var dbOperations = [];
              dbOperations.push(models.db.DataSet.findAll({
                attributes: ['id', 'active', 'data_series_id'],
                include: [
                  {
                    model: models.db.DataSetDcp,
                    attributes: ['position'],
                    required: true
                  }
                ]
              }));
              dbOperations.push(models.db.DataSet.findAll({
                attributes: ['id', 'active', 'data_series_id'],
                include: [
                  {
                    model: models.db.DataSetOccurrence,
                    required: true
                  }
                ]
              }));

              Promise.all(dbOperations).then(function(dataSetsArray) {
                dataSetsArray.forEach(function(dataSets) {
                  dataSets.forEach(function(dataSet) {
                    var dSetObject = {
                      id: dataSet.id,
                      data_series_id: dataSet.data_series_id,
                      active: dataSet.active
                    };

                    if (dataSet.DataSetDcp)
                      Object.assign(dSetObject, dataSet.DataSetDcp.get());
                    else if (dataSet.DataSetOccurrence) { }

                    self.data.dataSets.push(DataSetFactory.build(dSetObject));
                  });
                });

                self.isLoaded = true;
                resolve();
              }).catch(function(err) {
                clean();
                reject(err);
              });

            });

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
          var message = "Could not save project: ";
          if (e.errors)
            message += e.errors[0].message;
          reject(new exceptions.ProjectError(message));
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

  /**
   * It updates a project from given object values.
   *
   * @param {Object} projectObject - an javascript object containing project values
   * @return {Promise<DataFormat>} - a 'bluebird' module with DataFormat instance or error callback
   */
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

  removeProject: function(restriction) {
    var self = this;

    return new Promise(function(resolve, reject) {
      self.getProject(restriction).then(function(projectResult) {
        models.db["Project"].destroy({
          where: {
            id: projectResult.id
          }
        }).then(function() {
          for(var i = 0; i < self.data.projects.length; ++i) {
            if (self.data.projects[i].id == projectResult.id)
              self.data.projects.splice(i, 1);
          }
          resolve();
        }).catch(function(err) {
          console.log("Remove Project: ", err);
          reject(new exceptions.ProjectError("Could not remove project: " + err.message));
        })
      }).catch(function(err) {
        reject(err);
      })
    });

  },

  /**
   * It retrieves a Project list object from loaded projects.
   *
   * @return {Array} - a 'bluebird' module with Project instance or error callback
   */
  listProjects: function() {
    var projectList = [];
    this.data.projects.forEach(function(project) {
      projectList.push(Utils.clone(project));
    });
    return projectList;
  },

  /**
   * It saves ServiceInstance in database and storage it in memory
   * @param {Object} serviceObject - An object containing project values to be saved.
   * @return {Promise} - a 'bluebird' module. The callback is either a {ServiceInstance} data values or error
   */
  addServiceInstance: function(serviceObject) {
    return new Promise(function(resolve, reject){
      lock.writeLock(function(release) {
        models.db.ServiceInstance.create(serviceObject).then(function(serviceResult){
          resolve(serviceResult.get())
          release();
        }).catch(function(e) {
          var message = "Could not save service instance: ";
          if (e.errors)
            message += e.errors[0].message;
          reject(new Error(message));
          release();
        });
      });
    });
  },

  listServiceInstances: function(restriction) {
    return new Promise(function(resolve, reject){
      models.db.ServiceInstance.findAll({where: restriction}).then(function(services) {
        var output = [];
        services.forEach(function(service){
          output.push(service.get());
        });

        resolve(output);
      });
    });
  },
  
  getServiceInstance : function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject){
      self.listServiceInstances(restriction).then(function(result) {
        if (result.length == 0)
          return reject(new Error("No service instances found"));

        if (result.length > 1)
          return reject(new Error("More than one service instance retrieved"));
        
        resolve(result[0]);
      }).catch(function(err) {
        reject(err);
      });
    });
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

  /**
   * It retrieves a DataProviderType list object from database.
   *
   * @return {Promise<DataProviderType>} - a 'bluebird' module with DataProviderType instance or error callback
   */
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
   * It retrieves a DataFormats list object from database.
   *
   * @return {Promise<DataFormat>} - a 'bluebird' module with DataFormat instance or error callback
   */
  listDataFormats: function() {
    return new Promise(function(resolve, reject) {
      models.db.DataFormat.findAll({}).then(function(dataFormats) {
        var output = [];

        dataFormats.forEach(function(dataFormat){
          output.push(Utils.clone(dataFormat.get()));
        });

        resolve(output);
      }).catch(function(err) {
        reject(new exceptions.DataFormatError("Could not retrieve data format"));
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
   * It retrieves a DataSeriesSemantics object from restriction. It should be an object containing either id identifier or
   * name identifier. This operation must retrieve only a row.
   *
   * @param {Object} restriction - An object containing DataSeriesSemantics identifier to get it.
   * @return {Promise<DataSeriesSemantics>} - a 'bluebird' module with DataSeriesSemantics instance or error callback
   */
  getDataSeriesSemantics: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.listDataSeriesSemantics(restriction).then(function(semanticsList) {
        if (semanticsList.length === 1)
          return resolve(semanticsList[0]);

        // error getting more than one or none
        return reject(new exceptions.DataSeriesSemanticsError("DataSeriesSemantics not found"));
      }).catch(function(err) {
        reject(err);
      });
    });
  },

  /**
   * It retrieves a DataSeriesSemantics list object from restriction.
   *
   * @param {Object} restriction - An optional object containing DataSeriesSemantics identifier to filter it.
   * @return {Promise<DataSeriesSemantics>} - a 'bluebird' module with DataSeriesSemantics instance or error callback
   */
  listDataSeriesSemantics: function(restriction) {
    return new Promise(function(resolve, reject) {
      models.db.DataSeriesSemantics.findAll({where: restriction}).then(function(semanticsList) {
        var output = [];

        semanticsList.forEach(function(semantics) {
          output.push(Utils.clone(semantics.get()));
        });

        resolve(output);
      }).catch(function(err) {
        reject(new exceptions.DataSeriesSemanticsError("Could not retrieve data series semantics ", err));
      })
    });
  },

  /**
   * It saves DataProvider in database and load it in memory
   * @param {Object} dataProviderObject - An object containing needed values to create DataProvider object.
   * @return {Promise<DataProvider>} - a 'bluebird' module with DataProvider instance or error callback
   */
  addDataProvider: function(dataProviderObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataProvider.create(dataProviderObject).then(function(dataProvider){
        var dProvider = new DataProvider(dataProvider.get());
        self.data.dataProviders.push(dProvider);

        resolve(dProvider);

        //  todo: emit signal
        var d = new DataProvider(dProvider);
        d.data_provider_intent_name = 1;
        TcpManager.sendData({"DataProviders": [d.toObject()]});

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
   * @return {Promise<DataProvider>} - a 'bluebird' module with DataProvider instance or error callback
   */
  getDataProvider: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataProvider = getItemByParam(self.data.dataProviders, restriction);
      if (dataProvider)
        resolve(new DataProvider(dataProvider));
      else
        reject(new exceptions.DataProviderError("Could not find a data provider: " + restriction[Object.keys(restriction)[0]]));
    });
  },

  /**
   * It retrieves DataProviders loaded in memory.
   *
   * @param {Object} restriction - An object containing DataProvider filter values
   * @return {Array<DataProvider>} - An array with DataProviders available/loaded in memory.
   */
  listDataProviders: function(restriction) {
    var dataProviderObjectList = [];

    if (restriction === undefined || restriction === null)
      restriction = {};

    var keys = Object.keys(restriction);

    this.data.dataProviders.forEach(function(dataProvider) {

      // todo: implement better approach
      var keyCont = 0;
      keys.forEach(function (key) {
        if (dataProvider[key] === restriction[key])
          ++keyCont;
        else
          return false;
      });

      if (keyCont == keys.length)
        dataProviderObjectList.push(new DataProvider(dataProvider));

      keyCont = 0;
    });
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

          resolve(new DataProvider(dataProvider));
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
   * @param {bool} cascade - A bool value to delete on cascade
   * @return {Promise} - a 'bluebird' module with DataProvider instance or error callback
   */
  removeDataProvider: function(dataProviderParam, cascade) {
    var self = this;
    return new Promise(function(resolve, reject) {
      if (!cascade)
        cascade = false;

      for(var index = 0; index < self.data.dataProviders.length; ++index) {
        var provider = self.data.dataProviders[index];
        if (provider.id == dataProviderParam.id || provider.name == dataProviderParam.name) {
          models.db.DataProvider.destroy({where: {id: provider.id}}).then(function() {
            self.data.dataProviders.splice(index, 1);
            resolve();
          }).catch(function(err) {
            console.log(err);
            reject(new exceptions.DataProviderError("Could not remove DataProvider with a collector associated", err));
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
  getDataSeries: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataSerie = getItemByParam(self.data.dataSeries, restriction);
      if (dataSerie)
        resolve(new DataSeries(dataSerie));
      else
        reject(new exceptions.DataSeriesError("Could not find a data series: " + restriction[Object.keys(restriction)]));
    });
  },

  /**
   * It retrieves DataSeries loaded in memory.
   *
   * @param {Object} restriction - an object to filter result
   * @return {Promise<Array<DataSeries>>} - An array with DataSeries available/loaded in memory.
   */
  listDataSeries: function(restriction) {
    var self = this;

    return new Promise(function(resolve, reject) {
      var dataSeriesList = [];

      // todo: should have parent search module? #tempCode for filtering
      if (restriction && restriction.hasOwnProperty("Collector")) {
        // collector restriction
        self.listCollectors({}).then(function(collectorsResult) {

          // creating a copy
          var copyDataSeries = [];
          self.data.dataSeries.forEach(function(ds) {
            copyDataSeries.push(new DataSeries(ds));
          });

          copyDataSeries.forEach(function(element, index, arr) {
            collectorsResult.some(function(collector) {
              // collect
              if (collector.data_series_output == element.id) {
                arr.splice(index, 1);
                return true;
              } else if (collector.data_series_input == element.id) { // removing input dataseries
                arr.splice(index, 1);
              }
              return false;
            })
          });
          
          // collect output and processing
          return resolve(copyDataSeries);
        }).catch(function(err) {
          return reject(err);
        });

      } else if (restriction && restriction.hasOwnProperty('DataProvider')) {
        var dataProviderRestriction = restriction.DataProvider;

        var dataProviders = self.listDataProviders(dataProviderRestriction);

        self.data.dataSeries.forEach(function (dataSeries) {
          dataProviders.forEach(function (dataProvider) {
            if (dataSeries.data_provider_id === dataProvider.id)
              dataSeriesList.push(new DataSeries(dataSeries));
          });
        });

        return resolve(dataSeriesList);
      } else {
        self.data.dataSeries.forEach(function(dataSeries) {
          dataSeriesList.push(new DataSeries(dataSeries));
        });

        return resolve(dataSeriesList);
      }
    });
  },

  /**
   * It saves a DataSeries object in database. It also saves DataSet if there are. The object syntax is:
   * {
   *   modelValuesObject...
   *
   *   dataSets: [
   *     {
   *       semantics: {
   *         name: ...
   *         format_name: ...
   *         type_name: ...
   *       }
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
  addDataSeries: function(dataSeriesObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var output;
      models.db.DataSeries.create(dataSeriesObject).then(function(dataSerie){
        output = new DataSeries(dataSerie.get());

        var rollback = function(err) {
          dataSerie.destroy().then(function () {
            reject(err);
          });
        };

        // getting semantics
        dataSerie.getDataSeriesSemantic().then(function(dataSemantics) {
          // if there DataSets to save too
          if (dataSeriesObject.dataSets) {
            var dataSets = [];
            for(var i = 0; i < dataSeriesObject.dataSets.length; ++i) {
              var dSet = dataSeriesObject.dataSets[i];
              dSet.data_series_id = dataSerie.id;
              dataSets.push(self.addDataSet(dataSemantics.get(), dSet));
            }

            Promise.all(dataSets).then(function(dataSets){
              self.data.dataSeries.push(new DataSeries(output));
              // temp code: getting wkt
              
              var promises = [];
              
              dataSets.forEach(function(dSet) {
                promises.push(self.getDataSet({id: dSet.id}, Enums.Format.WKT));
              });
              
              Promise.all(promises).then(function(wktDataSets) {
                // todo: emit signal
                output.dataSets = wktDataSets;
                // TcpManager.sendData({"DataSeries": [output.toObject()]});

                // resolving promise
                resolve(output);
              }).catch(function(err) {
                reject(new exceptions.DataSetError("Could not retrieve data set dcp as wkt format. ", err));
              });
            }).catch(function(err) {
              rollback(err);
            });

          } else {
            // rollback dataseries
            rollback(new exceptions.DataSeriesError("Could not save DataSeries without any data set."));
          }
        }).catch(function(err) {
          rollback(err);
        });
      }).catch(function(err){
        var msg = "";

        // err.errors.forEach(function(error) {
        //   msg = error.message;
        // });
        console.log(err);
        reject(new exceptions.DataSeriesError(err.message));
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
        }).then(function() {
          dataSeries.name = dataSeriesObject.name;
          dataSeries.description = dataSeriesObject.description;

          resolve(new DataSeries(dataSeries));
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
        if (dataSeries.id == dataSeriesParam.id || dataSeries.name == dataSeriesParam.name) {
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
            reject(new exceptions.DataSeriesError("Could not remove DataSeries: ", err));
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
   *   "id" : INT,
   *   "data_series_id" : INT,
   *   "active" : BOOL,
   *   "format" : {...},
   *   "position" : STRING::WKT
   * }
   *
   * @param {string} dataSeriesSemantic - A string value representing DataSet type. (dcp, occurrence, grid).
   * @param {Array<Object>} dataSetObject - An object containing DataSet values to save it.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  addDataSet: function(dataSeriesSemantic, dataSetObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataSet.create({
        active: dataSetObject["active"],
        data_series_id: dataSetObject["data_series_id"]
      }).then(function(dataSet) {

        var onSuccess = function(dSet) {
          var output;
          output = DataSetFactory.build(Object.assign(Utils.clone(dSet.get()), dataSet.get()));

          output.semantics = dataSeriesSemantic;

          // save dataformat
          if (dataSetObject.format) {
            var formats = dataSetObject["format"];
            var formatList = [];

            if (formats instanceof Array) {
              formatList = formats;
            } else if (formats instanceof Object) {
              for(var key in formats) {
                if (formats.hasOwnProperty(key)) {
                  formatList.push({
                    data_set_id: dataSet.id,
                    key: key,
                    value: formats[key]
                  });
                }
              }
            } else {
              // invalid
            }

            models.db.DataSetFormat.bulkCreate(formatList, {data_set_id: dataSet.id}).then(function () {
              models.db.DataSetFormat.findAll({where: {data_set_id: dataSet.id}}).then(function(dataSetFormats) {
                output.format = {};
                dataSetFormats.forEach(function(dataSetFormat) {
                  output.format[dataSetFormat.key] = dataSetFormat.value;
                });

                self.data.dataSets.push(output);
                resolve(output);
              });
            }).catch(function (err) {
              reject(new exceptions.DataFormatError("Could not save data format: ", err));
            });
          } else {// todo: validate it
            self.data.dataSets.push(output);
            resolve(output);
          }
        };

        var onError = function(err) {
          reject(new exceptions.DataSetError("Could not save data set.", err));
        };

        // rollback data set function if any error occurred
        var rollback = function(dataSet) {
          dataSet.destroy().then(function() {
            console.log("rollback");
            reject(new exceptions.DataSetError("Invalid dataset type. DataSet destroyed"));
          }).catch(onError);
        };

        if (dataSeriesSemantic && dataSeriesSemantic instanceof Object) {
          switch(dataSeriesSemantic.data_series_type_name) {
            case DataSeriesType.DCP:
              var dataSetDcp = {
                data_set_id: dataSet.id,
                position: dataSetObject.position
              };
              models.db.DataSetDcp.create(dataSetDcp).then(onSuccess).catch(onError);
              break;
            case DataSeriesType.OCCURRENCE:
              models.db.DataSetOccurrence.create({data_set_id: dataSet.id}).then(onSuccess).catch(onError);
              break;
            case DataSeriesType.GRID:
              //  todo: implement it
              rollback(dataSet);
              break;
            case DataSeriesType.MONITORED:
            //  todo: implement it
              rollback(dataSet);
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
   * getDataSet({id: 1}, 'GeoJSON').then(function(dataSet) {
   *   ... do some operation
   * }).catch(function(err) {
   *   .. an exception occurred
   * });
   *
   * @param {Object} restriction - An object containing DataSet identifier to get it and type of DataSet.
   * @param {Format} format - A format to describe which format should be retrieve. Default: GeoJSON.
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  getDataSet: function(restriction, format) {
    var self = this;
    return new Promise(function(resolve, reject) {
      // setting default format
      if (format === undefined)
        format = Enums.Format.GEOJSON;

      if (format !== Enums.Format.GEOJSON && format !== Enums.Format.WKT)
        return reject(new exceptions.DataSetError("Invalid output format while retrieving dataset"));

      lock.readLock(function (release) {
        var dataSet = getItemByParam(self.data.dataSets, restriction);
        if (dataSet) {
          var output = DataSetFactory.build(dataSet);

          if (output.position && format === Enums.Format.WKT)
            // Getting wkt representation of Point from GeoJSON
            models.db.sequelize.query("SELECT ST_AsText(ST_GeomFromGeoJson('" + JSON.stringify(output.position) + "')) as geom").then(function(wktGeom) {
              // it retrieves an array with data result (array) and query executed.
              // if data result is empty or greater than 1, its not allowed.
              if (wktGeom[0].length !== 1)
                reject(new exceptions.DataSetError("Invalid wkt retrieved from geojson."));
              else {
                output.position = wktGeom[0][0].geom;
                resolve(output);
                release();
              }
            }).catch(function(err) {
              reject(err);
              release();
            });
          else {
            resolve(output);
            release();
          }
        }
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
     *   id: someValue
     * }
     *
     * dataSetObject => {
     *   active: ...,
     *   position: ...,
     *   ...
     * }
     */

    var self = this;
    return new Promise(function(resolve, reject) {

      var dataSet = getItemByParam(self.data.dataSets, restriction);

      if (dataSet) {

        models.db.DataSet.find({id: dataSet.id}).then(function(result) {
          result.updateAttributes({active: dataSetObject.active}).then(function() {
            result.getDataSet(restriction.semantics.data_series_type_name).then(function(dSet) {
              dSet.updateAttributes(dataSetObject).then(function() {
                var output = Utils.clone(result.get());
                output.class = "DataSet";
                switch (restriction.semantics.data_series_type_name) {
                  case DataSeriesType.DCP:
                    output.position = Utils.clone(dSet.position);
                    break;
                  default:
                    ;
                }
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
   * It removes a DataSet from database.
   *
   * @param {Object} dataSetId - An object containing Dataset identifier. {id: someValue}
   * @return {Promise} - a 'bluebird' module callback
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
  },

  addDataSeriesAndCollector: function(dataSeriesObject, scheduleObject, filterObject, serviceObject) {
    var self = this;

    return new Promise(function(resolve, reject) {

      var rollback = function(model, instance) {
        return model.destroy({
          where: {
            id: instance.id
          }
        })
      };

      var rollbackModels = function(models, instances, exception) {
        var promises = [];
        for(var i = 0; i < models.length; ++i) {
          promises.push(rollback(models[i], instances[i]));
        }

        Promise.all(promises).then(function() {
          console.log("Rollback all");
          return reject(exception);
        }).catch(function(err) {
          reject(err);
        })
      };

      self.addDataSeries(dataSeriesObject.input).then(function(dataSeriesResult) {
        self.addDataSeries(dataSeriesObject.output).then(function(dataSeriesResultOutput) {
          self.addSchedule(scheduleObject).then(function(scheduleResult) {
            var schedule = new Schedule(scheduleResult);
            var collectorObject = {};
  
            // todo: get service_instance id and collector status (active)
            collectorObject.data_series_input = dataSeriesResult.id;
            collectorObject.data_series_output = dataSeriesResultOutput.id;
            collectorObject.service_instance_id = serviceObject.id;
            collectorObject.schedule_id = scheduleResult.id;
            collectorObject.active = true;
            collectorObject.collector_type = 1;
            collectorObject.schedule_id = scheduleResult.id;
  
            self.addCollector(collectorObject, filterObject).then(function(collectorResult) {
              var collector = new Collector(collectorResult);
              var input_output_map = [];

              for(var i = 0; i < dataSeriesResult.dataSets.length; ++i) {
                var inputDataSet = dataSeriesResult.dataSets[i];
                var outputDataSet;
                if (dataSeriesResultOutput.dataSets.length == 1)
                  outputDataSet = dataSeriesResultOutput.dataSets[0];
                else
                  outputDataSet = dataSeriesResultOutput.dataSets[i];

                input_output_map.push({
                  input: inputDataSet.id,
                  output: outputDataSet.id
                });
              }

              collector.input_output_map = input_output_map;
              collector.schedule = schedule.toObject()

              resolve({collector: collector, input: dataSeriesResult, output:dataSeriesResultOutput, schedule: schedule});
              // resolve([dataSeriesResult, dataSeriesResultOutput])
            }).catch(function(err) {
              // rollback schedule
              rollbackModels([models.db.Schedule, models.db.DataSeries], [scheduleResult, dataSeriesResult], err);
            });
          }).catch(function(err) {
            // rollback dataseries
            rollbackModels([models.db.DataSeries, models.db.DataSeries], [dataSeriesResultOutput, dataSeriesResult], err);
          });
        }).catch(function(err) {
          rollbackModels([models.db.DataSeries], [dataSeriesResult], err);
        })
      }).catch(function(err) {
        reject(err);
      })  
    });
  },

  addSchedule: function(scheduleObject) {
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.Schedule.create(scheduleObject).then(function(schedule) {
        resolve(new Schedule(schedule.get()));
      }).catch(function(err) {
        // todo: improve error message
        reject(new exceptions.ScheduleError("Could not save schedule. ", err));
      });
    });
  },

  listSchedules: function(restriction) {
    var self = this;
    // todo: implement it
    return new Promise(function(resolve, reject) {
      resolve();
    });
  },

  getSchedule: function(restriction) {
    var self = this;
    // todo: implement it from listSchedules
    return new Promise(function(resolve, reject) {
      resolve();
    });
  },

  addCollector: function(collectorObject, filterObject) {
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.Collector.create(collectorObject).then(function(collectorResult) {
        if (_.isEmpty(filterObject))
          return resolve(collectorResult.get());


        filterObject.collector_id = collectorResult.id;

        self.addFilter(filterObject).then(function(filterResult) {
          var output = Utils.clone(collectorResult.get());
          output.filter = filterResult;

          resolve(output);
        }).catch(function(err) {
          console.log(err);
          reject(new exceptions.CollectorError("Could not save collector: ", err));
        })
      }).catch(function(err) {
        console.log(err);
        reject(new exceptions.CollectorError("Could not save collector: ", err));
      })
    });
  },

  listCollectors: function(restriction) {
    return new Promise(function(resolve, reject) {
      models.db['Collector'].findAll({where: restriction}).then(function(collectorsResult) {
        var output = [];
        collectorsResult.forEach(function(collector) {
          output.push(collector.get());
        });
        resolve(output);
      }).catch(function(err) {
        console.log(err);
        reject(new exceptions.CollectorError("Could not retrieve collector: " + err.message));
      });
    });
  },

  addFilter: function(filterObject) {
    var self = this;

    return new Promise(function(resolve, reject) {
      var filterValues = {collector_id: filterObject.collector_id};
      // checking filter by date
      if (filterObject.hasOwnProperty('date') && !_.isEmpty(filterObject.date)) {
        filterValues.discard_before = new Date(filterObject.date.beforeDate);
        filterValues.discard_after = new Date(filterObject.date.afterDate);
      }

      // checking filter by area
      if (filterObject.hasOwnProperty('area') && !_.isEmpty(filterObject.area)) {
        filterValues.region = {
          "type": "Polygon",
          "coordinates": [
            [
              [filterObject.area["minX"], filterObject.area["minY"]],
              [filterObject.area["maxX"], filterObject.area["minY"]],
              [filterObject.area["maxX"], filterObject.area["maxY"]],
              [filterObject.area["minX"], filterObject.area["maxY"]],
              [filterObject.area["minX"], filterObject.area["minY"]]
            ]
          ],
          "crs": {
            "type": "name",
            "properties": {
              "name": "EPSG:4326"
            }
          }
        };
      }

      // checking filter
      models.db.Filter.create(filterValues).then(function(filter) {
        resolve(filter.get());
      }).catch(function(err) {
        // todo: improve error message
        reject(new Error("Could not save filter. ", err));
      });
    });
  },

  addAnalysis: function(analysisObject) {
    return new Promise(function(resolve, reject) {
      // todo: make it as factory: AnalysisGrid, Analysis...
      models.db["Analysis"].create(analysisObject).then(function(analysisResult) {
        var analysisDataSeriesObject = Utils.clone(analysisObject.analysisDataSeries);
        var analysisObject = new Analysis(analysisResult);
        models.db["AnalysisDataSeries"].create(analysisDataSeriesObject).then(function(analysisDataSeriesResult) {
          var analysisDataSeries = new AnalysisDataSeries(analysisDataSeriesResult);

          console.log(analysisResult.get());
          console.log(analysisDataSeriesResult.get());
          resolve(analysisResult.get());
        }).catch(function(err) {
          Utils.rollbackModels(models.db['Analysis'], analysisResult.get(), new exceptions.AnalysisError("Could not save Analysis Dataseries: " + err), {reject: reject})
        });
      }).catch(function(err) {
        var msg = "";

        err.errors.forEach(function(error) {
          msg += error.message + "\n";
        });
        reject(new exceptions.AnalysisError("Could not save analysis: " + msg));
      })
    });
  }

};

module.exports = DataManager;