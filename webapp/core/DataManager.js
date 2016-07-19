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
var fs = require('fs');
var path = require('path');

// Tcp
var TcpManager = require('./TcpManager');

// data model
var DataModel = require('./data-model');

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

function _processFilter(filterObject) {
  var filterValues = {};
  // checking filter by date
  if (filterObject.hasOwnProperty('date') && !_.isEmpty(filterObject.date)) {
    if (filterObject.date.beforeDate)
      filterValues.discard_before = new Date(filterObject.date.beforeDate);
    if (filterObject.date.afterDate)
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
  return filterValues;
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

  Promise: Promise,
  TcpManager: TcpManager,
  DataModel: DataModel,

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

      // console.log("Loading DAO's...");
      // for(var k in dao) {
      //   if (dao.hasOwnProperty(k)) {
      //     var klass = dao[k](self);
      //     self[k] = new klass();
      //   }
      // }

      var fn = function() {
        // todo: insert default values in database
        var inserts = [];

        // default users
        var salt = models.db['User'].generateSalt();
        inserts.push(models.db['User'].create({
          name: "TerraMA2 User",
          username: "terrama2",
          password: models.db['User'].generateHash("terrama2", salt),
          salt: salt,
          cellphone: '14578942362',
          email: 'terrama2@terrama2.inpe.br',
          administrator: false
        }));

        // services type
        inserts.push(models.db.ServiceType.create({name: "COLLECT"}));
        inserts.push(models.db.ServiceType.create({name: "ANALYSIS"}));

        // default services
        var collectorService = {
          name: "Local Collector",
          description: "Local service for Collect",
          port: 6543,
          pathToBinary: "terrama2_service",
          numberOfThreads: 0,
          service_type_id: Enums.ServiceType.COLLECTOR,
          log: {
            host: "127.0.0.1",
            port: 5432,
            user: "postgres",
            password: "postgres",
            database: "nodejs" // TODO: change it
          }
        };

        var analysisService = Object.assign({}, collectorService);
        analysisService.name = "Local Analysis";
        analysisService.description = "Local service for Analysis";
        analysisService.port = 6544;
        analysisService.service_type_id = Enums.ServiceType.ANALYSIS;

        inserts.push(self.addServiceInstance(collectorService));
        inserts.push(self.addServiceInstance(analysisService));

        // data provider type defaults
        inserts.push(self.addDataProviderType({id: 1, name: "FILE", description: "Desc File"}));
        inserts.push(self.addDataProviderType({id: 2, name: "FTP", description: "Desc Type1"}));
        inserts.push(self.addDataProviderType({id: 3, name: "HTTP", description: "Desc Http"}));
        inserts.push(self.addDataProviderType({id: 4, name: "POSTGIS", description: "Desc Postgis"}));

        // data provider intent defaults
        inserts.push(models.db.DataProviderIntent.create({
          id: Enums.DataProviderIntentId.COLLECT,
          name: "COLLECT",
          description: "Desc Collect intent"
        }));

        inserts.push(models.db.DataProviderIntent.create({
          id: Enums.DataProviderIntentId.PROCESSING,
          name: "PROCESSING",
          description: "Desc Processing intent"
        }));

        // data series type defaults
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.DCP, description: "Data Series DCP type"}));
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.OCCURRENCE, description: "Data Series Occurrence type"}));
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.GRID, description: "Data Series Grid type"}));
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.ANALYSIS_MONITORED_OBJECT, description: "Data Series Analysis Monitored Object"}));
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.STATIC_DATA, description: "Data Series Static Data"}));

        // data formats semantics defaults todo: check it
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.CSV, description: "DCP description"}));
        inserts.push(self.addDataFormat({name: DataSeriesType.OCCURRENCE, description: "Occurrence description"}));
        inserts.push(self.addDataFormat({name: DataSeriesType.GRID, description: "Grid Description"}));
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.POSTGIS, description: "POSTGIS description"}));
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.OGR, description: "Gdal ogr"}));
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.GEOTIFF, description: "GeoTiff"}));

        // analysis type
        inserts.push(models.db["AnalysisType"].create({id: Enums.AnalysisType.DCP, name: "Dcp", description: "Description Dcp"}));
        inserts.push(models.db["AnalysisType"].create({id: Enums.AnalysisType.GRID, name: "Grid", description: "Description Grid"}));
        inserts.push(models.db["AnalysisType"].create({id: Enums.AnalysisType.MONITORED, name: "Monitored Object", description: "Description Monitored"}));

        // analysis data series type
        inserts.push(models.db["AnalysisDataSeriesType"].create({
          id: Enums.AnalysisDataSeriesType.DATASERIES_DCP_TYPE,
          name: "Dcp",
          description: "Description Dcp"
        }));
        inserts.push(models.db["AnalysisDataSeriesType"].create({
          id: Enums.AnalysisDataSeriesType.DATASERIES_GRID_TYPE,
          name: "Grid",
          description: "Description Grid"
        }));

        inserts.push(models.db["AnalysisDataSeriesType"].create({
          id: Enums.AnalysisDataSeriesType.DATASERIES_MONITORED_OBJECT_TYPE,
          name: "Monitored Object",
          description: "Description Monitored"
        }));

        inserts.push(models.db["AnalysisDataSeriesType"].create({
          id: Enums.AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE,
          name: "Additional Data",
          description: "Description Additional Data"
        }));

        // script language supported
        inserts.push(models.db["ScriptLanguage"].create({id: Enums.ScriptLanguage.PYTHON, name: "PYTHON"}));
        inserts.push(models.db["ScriptLanguage"].create({id: Enums.ScriptLanguage.LUA, name: "LUA"}));

        // semantics: temp code: TODO: fix
        var semanticsJsonPath = path.join(__dirname, "../../share/terrama2/semantics.json");
        var semanticsObject = JSON.parse(fs.readFileSync(semanticsJsonPath, 'utf-8'));

        // storing semantics providers dependency
        var semanticsWithProviders = {};

        semanticsObject.forEach(function(semanticsElement) {
          semanticsWithProviders[semanticsElement.code] = semanticsElement.providers_type_list;
          inserts.push(self.addDataSeriesSemantics({
            code: semanticsElement.code,
            name: semanticsElement.name,
            data_format_name: semanticsElement.format,
            data_series_type_name: semanticsElement.type
          }));
        });

        // it will match each of semantics with providers
        var insertSemanticsProvider = function() {
          self.listSemanticsProvidersType().then(function(result) {
            if (result.length != 0) {
              releaseCallback();
              return;
            }

            self.listDataProviderType().then(function(dataProvidersType) {
              self.listDataSeriesSemantics().then(function(semanticsList) {
                var semanticsProvidersArray = [];
                semanticsList.forEach(function(semantics) {
                  var dependencies = semanticsWithProviders[semantics.code] || [];

                  dependencies.forEach(function(dependency) {
                    dataProvidersType.some(function(providerType) {
                      if (dependency == providerType.name) {
                        semanticsProvidersArray.push({
                          data_provider_type_id: providerType.id,
                          data_series_semantics_id: semantics.id
                        });
                        return true;
                      }
                    });
                  })
                });

                // adding metadata to semantics
                var semanticsMetadataArray = [];
                semanticsList.forEach(function(semantics) {
                  semanticsObject.some(function(element) {
                    if (semantics.code == element.code) {
                      if (element.metadata) {
                        for(var key in element.metadata) {
                          if (element.metadata.hasOwnProperty(key)) {
                            semanticsMetadataArray.push({
                              key: key,
                              value: element.metadata[key],
                              data_series_semantics_id: semantics.id
                            });
                          }
                        }
                      }
                      return true;
                    }
                  })
                });

                var _makeSemanticsMetadata = function() {
                  models.db["SemanticsMetadata"].bulkCreate(semanticsMetadataArray).then(function(res) {
                    releaseCallback();
                  }).catch(function(err) {
                    releaseCallback();
                  })
                }

                models.db["SemanticsProvidersType"].bulkCreate(semanticsProvidersArray).then(function(result) {
                  _makeSemanticsMetadata();
                }).catch(function(err) {
                  _makeSemanticsMetadata();
                })
              }).catch(function(err) {
                console.log(err);
                releaseCallback();
              });
            }).catch(function(err) {
              console.log(err);
              releaseCallback();
            });
          }).catch(function() {
            releaseCallback();
          });
        };

        Promise.all(inserts).then(function() {
          insertSemanticsProvider();
        }).catch(function(err) {
          console.log(err);
          insertSemanticsProvider()
        });
      };

      connection.authenticate().then(function() {
        connection.sync().then(function () {
          fn();
        }, function() {
          fn();
        }).catch(function(err) {
          console.log(err);
          fn();
        });
      }).catch(function(err) {
        release();
        callback(new Error("Could not initialize terrama2 due: " + err.message));
      })
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
      // helper for clean up datamanager and reject promise
      var _rejectClean = function(err) {
        clean();
        console.log("CLEAN: ", err);
        reject(err);
      };

      var _continueInMemory = function(dataSetObject, dataSetFmtsResult, builtDataSeries) {
        // for each format
        var fmt = {};
        dataSetFmtsResult.forEach(function(format) {
          if (format.data_set_id == dataSetObject.id) {
            fmt[format.key] = format.value;
          }
        });

        var builtDataSet = DataModel.DataSetFactory.build(Object.assign(dataSetObject, {format: fmt}));

        builtDataSeries.dataSets.push(builtDataSet.toObject());
        // adding local cache. TODO: Is it necessary to store individual or along data series?
        self.data.dataSets.push(builtDataSet);
      };

      models.db.Project.findAll({}).then(function(projects) {
        projects.forEach(function(project) {
          self.data.projects.push(project.get());
        });

        models.db.DataProvider.findAll({include: [models.db['DataProviderType']]}).then(function(dataProviders){
          dataProviders.forEach(function(dataProvider) {
            var provider = new DataModel.DataProvider(dataProvider.get());
            self.data.dataProviders.push(provider);
          });

          // find all data series
          models.db.DataSeries.findAll({
            include: [
              models.db['DataSeriesSemantics'],
              {
                model: models.db['DataSet'],
                include: [
                  {
                    model: models.db['DataSetDcp'],
                    attributes: ['position'],
                    required: false
                  },
                  {
                    model: models.db.DataSetOccurrence,
                    required: false
                  },
                  {
                    model: models.db.DataSetMonitored,
                    required: false
                  },
                  models.db['DataSetFormat']
                ]
              }
            ]
          }).then(function(dataSeries) {
            var _setWKT = function(data, wkt) {
              data.position = wkt;
              self.data.dataSets.push(data);
            };
            dataSeries.forEach(function(dSeries) {
              var builtDataSeries = new DataModel.DataSeries(dSeries.get());

              builtDataSeries.dataSets.forEach(function(dSet) {
                if (dSet.position) {
                  self.getWKT(dSet.position).then(function(wktPosition) {
                    _setWKT(dSet, wktPosition);
                  }).catch(_rejectClean);
                } else
                  self.data.dataSets.push(dSet);
              });

              self.data.dataSeries.push(builtDataSeries);
            });


            self.isLoaded = true;
            resolve();

          }).catch(_rejectClean);
        }).catch(_rejectClean);
      }).catch(_rejectClean);
    });
  },

  getWKT: function(geoJSONObject) {
    return new Promise(function(resolve, reject) {
      models.db.sequelize.query("SELECT ST_AsText(ST_GeomFromGeoJson('" + JSON.stringify(geoJSONObject) + "')) as geom").then(function(wktGeom) {
        // it retrieves an array with data result (array) and query executed.
        // if data result is empty or greater than 1, its not allowed.
        if (wktGeom[0].length !== 1)
          reject(new exceptions.DataSetError("Invalid wkt retrieved from geojson."));
        else {
          resolve(wktGeom[0][0].geom);
        }
      }).catch(function(err) {
        reject(err);
      });
    })
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
          reject(new exceptions.ProjectError("Could not remove project with data provider associated"));
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
          var service = new DataModel.Service(serviceResult);
          var logObject = serviceObject.log;
          logObject.service_instance_id = serviceResult.id;
          models.db['Log'].create(logObject).then(function(logResult) {
            var log = new DataModel.Log(logResult);
            service.log = log.toObject();

            resolve(service);
            release();
          }).catch(function(err) {
            console.log(err);
            Utils.rollbackPromises([serviceResult.destroy()], new Error("Could not save log: " + err.message), reject);
            release();
          });

        }).catch(function(e) {
          console.log(e);
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
      models.db['ServiceInstance'].findAll({
        where: restriction,
        include: [models.db.Log]
      }).then(function(services) {
        var output = [];
        services.forEach(function(service){
          var serviceObject = new DataModel.Service(service.get());
          var logObject = new DataModel.Log(service.Log || {});
          serviceObject.log = logObject;
          output.push(serviceObject);
        });

        resolve(output);
      }).catch(function(err) {
        console.log(err);
        reject(new Error("Could not retrieve services " + err.message));
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

  removeServiceInstance: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getServiceInstance(restriction).then(function(serviceResult) {
        // update collectors removing ID and setting them to inactive
        self.updateCollectors({service_instance_id: serviceResult.id}, {active: false}).then(function() {
          models.db['ServiceInstance'].destroy({where: restriction}).then(function() {
            resolve();
          }).catch(function(err) {
            console.log(err);
            reject(new Error("Could not remove service instance. " + err.message));
          });
        }).catch(function(err) {
          reject(err);
        });
      }).catch(function(err) {
        reject(err);
      })

    });
  },

  updateServiceInstance: function(serviceId, serviceObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getServiceInstance({id: serviceId}).then(function(serviceResult) {
        models.db['ServiceInstance'].update(serviceObject, {
          fields: ['name', 'description', 'port', 'numberOfThreads', 'runEnviroment', 'host', 'sshUser', 'sshPort', 'pathToBinary'],
          where: {
            id: serviceId
          }
        }).then(function() {
          resolve();
        }).catch(function(err) {
          reject(err);
        })
      }).catch(function(err) {
        reject(err);
      })
    });
  },

  updateLog: function(logId, logObject) {
    return new Promise(function(resolve, reject) {
      models.db.Log.update(logObject, {
        fields: ['host', 'port', 'user', 'database'],
        where: {
          id: logId
        }
      }).then(function() {
        resolve();
      }).catch(function(err) {
        reject(new Error("Could not update log " + err.toString()));
      })
    })
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
      models.db['DataProviderType'].create(dataProviderTypeObject).then(function(result) {
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
      models.db['DataProviderType'].findAll({}).then(function(result) {
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
   * It retrieves a DataProviderType object from database.
   *
   * @return {Promise<DataProviderType>} - a 'bluebird' module with DataProviderType instance or error callback
   */
  getDataProviderType: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db['DataProviderType'].findOne({where: restriction}).then(function(typeResult) {
        resolve(typeResult.get());
      }).catch(function(err) {
        console.log(err);
        reject(new Error("Could not retrieve DataProviderType " + err.message));
      })
    });
  },

  /**
   * It retrieves a DataProviderIntent object from database.
   *
   * @return {Promise<DataProviderIntent>} - a 'bluebird' module with DataProviderIntent instance or error callback
   */
  getDataProviderIntent: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db['DataProviderIntent'].findOne({where: restriction}).then(function(intentResult) {
        resolve(intentResult.get());
      }).catch(function(err) {
        console.log(err);
        reject(new Error("Could not retrieve DataProviderIntent " + err.message));
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
      models.db['DataFormat'].create(dataFormatObject).then(function(result) {
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
      models.db['DataFormat'].findAll({}).then(function(dataFormats) {
        var output = [];

        dataFormats.forEach(function(dataFormat){
          output.push(Utils.clone(dataFormat.get()));
        });

        resolve(output);
      }).catch(function(err) {
        reject(new exceptions.DataFormatError("Could not retrieve data format", err));
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

  listSemanticsProvidersType: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db['SemanticsProvidersType'].findAll({where: restriction}).then(function(semanticsProvidersResult) {
        var output = [];
        semanticsProvidersResult.forEach(function(element) {
          output.push(element.get());
        })
        resolve(output);
      }).catch(function(err) {
        console.log(err);
        reject(err);
      });
    })
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

        dataProvider.getDataProviderType().then(function(dataProviderType) {
          var dProvider = new DataModel.DataProvider(dataProvider.get());
          dProvider.data_provider_type = dataProviderType.get();
          self.data.dataProviders.push(dProvider);

          resolve(dProvider);

          //  todo: emit signal
          var d = new DataModel.DataProvider(dProvider);

          // todo: improve it. it should be set in web interface.
          // sending to all services
          self.listServiceInstances().then(function(servicesInstance) {
            var dataToSend = {"DataProviders": [d.toObject()]};

            servicesInstance.forEach(function(service) {
              try {
                TcpManager.emit('sendData', service, dataToSend);
              } catch (e) {

              }
            });

          }).catch(function(err) {
            reject(err);
          });
        }).catch(function(err) {
          console.log(err);
          reject(err);
        })

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
        resolve(new DataModel.DataProvider(dataProvider));
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
        dataProviderObjectList.push(new DataModel.DataProvider(dataProvider));

      keyCont = 0;
    });
    return dataProviderObjectList;
  },

  /**
   * It updates a DataProvider instance from object.
   *
   * @param {int} dataProviderId - A DataProvider identifier to get it.
   * @param {Object} dataProviderObject - An object containing DataProvider to update.
   * @return {Promise} - a 'bluebird' module with DataProvider instance or error callback
   */
  updateDataProvider: function(dataProviderId, dataProviderObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataProvider = getItemByParam(self.data.dataProviders, {id: dataProviderId});

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

          self.listServiceInstances().then(function(services) {
            services.forEach(function(service) {
              try {
                console.log("Sending Add_signal to update");
                TcpManager.emit('sendData', service, {
                  "DataProviders": [dataProvider.toObject()]
                })
              } catch (e) {

              }
            })
          }).catch(function(err) { });

          resolve(new DataModel.DataProvider(dataProvider));
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
            var objectToSend = {
              "DataProvider": [provider.id],
              "DataSeries": []
            };
            self.listServiceInstances().then(function(services) {
              // remove data series
              self.data.dataSeries.forEach(function(dataSerie, dataSerieIndex, dataSerieArr) {
                if (dataSerie.data_provider_id == provider.id) {
                  // setting in object
                  objectToSend.DataSeries.push(dataSerie.id);
                  // remove it from memory
                  self.data.dataSets.forEach(function(dataset, datasetIndex, datasetArr) {
                    if (dataset.data_series_id === dataSerie.id) {
                      datasetArr.splice(datasetIndex, 1);
                    }
                  });
                  dataSerieArr.splice(dataSerieIndex, 1);
                }
              });
              self.data.dataProviders.splice(index, 1);

              // sending Remove signal
              services.forEach(function(service) {
                try {
                  TcpManager.emit('removeData', service, objectToSend);
                } catch (e) {
                  console.log(e);
                }
              });

              resolve();
            }).catch(function(err) {
              console.log(err);
              reject(err);
            })
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
        resolve(new DataModel.DataSeries(dataSerie));
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

      if (restriction && restriction.hasOwnProperty("schema")) {
        if (restriction.schema == "all") {
          self.listDataSeries({"Collector": {}}).then(function(data) {
            self.listDataSeries({"DataSeries": "Static"}).then(function(staticData) {
              var output = [];
              data.forEach(function(d) {
                output.push(d);
              })

              staticData.forEach(function(d) {
                output.push(d);
              })

              return resolve(output);
            }).catch(function(err) {
              reject(err);
            });
          }).catch(function(err) {
            reject(err);
          });
        }
      } else if (restriction && restriction.hasOwnProperty('DataSeries')) {

        var _exec = function(func) {
          // get all dynamic
          var eDataSeries = [];
          self.data.dataSeries.forEach(function(dSeries) {
            var dSeriesSemantics = dSeries.data_series_semantics;
            if (func(dSeriesSemantics.data_series_type_name))
              eDataSeries.push(new DataModel.DataSeries(dSeries));
          });
          return resolve(eDataSeries);
        };

        switch (restriction.DataSeries) {
          case "Static":
            // get all static
            _exec(function(dataSeriestype) {
              return dataSeriestype == "STATIC_DATA";
            });
            break;
          case "Dynamic":
            // get all dynamic
            _exec(function(dataSeriestype) {
              return dataSeriestype != "STATIC_DATA";
            });
            break;
          default:
            return reject(new exceptions.DataSeriesError("Invalid data series restriction. Expected Static or Dynamic. \"" + restriction.DataSeries + "\""));
        }

      } else if (restriction && restriction.hasOwnProperty("Collector")) {
        // todo: should have parent search module? #tempCode for filtering
        // collector restriction
        self.listCollectors({}).then(function(collectorsResult) {

          if (collectorsResult.length == 0)
            return resolve(collectorsResult);

          // creating a copy
          var copyDataSeries = [];
          self.data.dataSeries.forEach(function(ds) {
            if (ds.data_series_semantics.data_series_type_name != Enums.DataSeriesType.STATIC_DATA)
              copyDataSeries.push(new DataModel.DataSeries(ds));
          });

          var output = [];

          copyDataSeries.forEach(function(element, index, arr) {
            console.log(element.name);
            collectorsResult.some(function(collector) {
              // collect
              if (collector.output_data_series === element.id) {
                return true;
              } else if (collector.input_data_series === element.id) {
                // removing input data series. TODO: improve this approach
                delete copyDataSeries[index];
                return true;
              }
              return false;
            })
          });

          // removing holes in array
          copyDataSeries.forEach(function(copyDs) {
            if (copyDs)
              output.push(copyDs);
          });


          // collect output and processing
          return resolve(output);
        }).catch(function(err) {
          return reject(err);
        });

      } else if (restriction && restriction.hasOwnProperty('DataSeriesSemantics')) {
        var semanticsRestriction = restriction.DataSeriesSemantics;

        self.data.dataSeries.forEach(function (dataSeries) {
          if (Utils.matchObject(semanticsRestriction, dataSeries.data_series_semantics)) {
            dataSeriesList.push(new DataModel.DataSeries(dataSeries));
          }
        });

        return resolve(dataSeriesList);
      } else {
        self.data.dataSeries.forEach(function(dataSeries) {
          dataSeriesList.push(new DataModel.DataSeries(dataSeries));
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
  addDataSeries: function(dataSeriesObject, analysisType) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var output;
      models.db.DataSeries.create(dataSeriesObject).then(function(dataSerie){
        var obj = dataSerie.get();

        var rollback = function(err) {
          dataSerie.destroy().then(function () {
            reject(err);
          });
        };

        // getting semantics
        dataSerie.getDataSeriesSemantic().then(function(dataSemantics) {
          obj['DataSeriesSemantic'] = dataSemantics;
          output = new DataModel.DataSeries(obj);

          // if there DataSets to save too
          if (dataSeriesObject.dataSets || dataSeriesObject.dataSets.length > 0) {
            var dataSets = [];
            for(var i = 0; i < dataSeriesObject.dataSets.length; ++i) {
              var dSet = dataSeriesObject.dataSets[i];
              dSet.data_series_id = dataSerie.id;
              dataSets.push(self.addDataSet(dataSemantics.get(), dSet, analysisType));
            }

            Promise.all(dataSets).then(function(dataSets){
              var dataSeriesInstance = new DataModel.DataSeries(output);
              dataSeriesInstance.dataSets = dataSets;
              self.data.dataSeries.push(dataSeriesInstance);
              // temp code: getting wkt

              var promises = [];

              dataSets.forEach(function(dSet) {
                promises.push(self.getDataSet({id: dSet.id}, Enums.Format.WKT));
              });

              Promise.all(promises).then(function(wktDataSets) {
                // todo: emit signal
                // self.data.dataSeries.push(new DataModel.DataSeries(output));
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
        reject(new exceptions.DataSeriesError("Could not save data series " + err.message));
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
  updateDataSeries: function(dataSeriesId, dataSeriesObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataSeries = getItemByParam(self.data.dataSeries, {id: dataSeriesId});

      if (dataSeries) {
        models.db.DataSeries.update(dataSeriesObject, {
          fields: ['name', 'description', 'data_provider_id'],
          where: {
            id: dataSeriesId
          }
        }).then(function() {
          self.getDataProvider({id: dataSeriesObject.data_provider_id}).then(function(dataProvider) {
            dataSeries.name = dataSeriesObject.name;
            dataSeries.description = dataSeriesObject.description;
            dataSeries.data_provider_id = dataProvider.id;

            resolve(new DataModel.DataSeries(dataSeries));
          }).catch(function(err) {
            console.log(err);
            reject(err);
          })
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
            id: dataSeriesParam.id
          }}).then(function (status) {
            self.data.dataSets.forEach(function(dSet, dSetIndex, array) {
              if (dSet.data_series_id === dataSeries.id)
                self.data.dataSets.splice(dSetIndex, 1);
            });
            self.data.dataSeries.splice(index, 1);
            resolve(status);
          }).catch(function (err) {
            console.log(err);
            reject(new exceptions.DataSeriesError("Could not remove DataSeries " + err.message));
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
  addDataSet: function(dataSeriesSemantic, dataSetObject, analysisType) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataSet.create({
        active: dataSetObject["active"],
        data_series_id: dataSetObject["data_series_id"]
      }).then(function(dataSet) {

        var onSuccess = function(dSet) {
          var output;
          output = DataModel.DataSetFactory.build(Object.assign(Utils.clone(dSet.get()), dataSet.get()));
          console.log(output)

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

                //  if analysis, add input dataseries id
                if (analysisType && analysisType.data_series_id)
                  output.format['monitored_object_dataseries_id'] = analysisType.data_series_id;

                self.data.dataSets.push(output);
                resolve(output);
              });
            }).catch(function (err) {
              console.log(err);
              reject(new exceptions.DataFormatError("Could not save data format: ", err));
            });
          } else {// todo: validate it
            self.data.dataSets.push(output);
            resolve(output);
          }
        };

        var onError = function(err) {
          console.log(err);
          reject(new exceptions.DataSetError("Could not save data set." + err.message));
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
            case DataSeriesType.STATIC_DATA:
              onSuccess(dataSet);
              break;
            case DataSeriesType.GRID:
              models.db.DataSetGrid.create({data_set_id: dataSet.id}).then(onSuccess).catch(onError);

              break;
            case DataSeriesType.ANALYSIS_MONITORED_OBJECT:
              models.db.DataSetMonitored.create({data_set_id: dataSet.id}).then(onSuccess).catch(onError);

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
          var output = DataModel.DataSetFactory.build(dataSet);

          if (output.position && format === Enums.Format.WKT)
            // Getting wkt representation of Point from GeoJSON
            self.getWKT(output.position).then(function(wktGeom) {
              resolve(output);
              release();
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

  addDataSeriesAndCollector: function(dataSeriesObject, scheduleObject, filterObject, serviceObject, intersectionArray, active) {
    var self = this;

    return new Promise(function(resolve, reject) {
      self.addDataSeries(dataSeriesObject.input).then(function(dataSeriesResult) {
        self.addDataSeries(dataSeriesObject.output).then(function(dataSeriesResultOutput) {
          self.addSchedule(scheduleObject).then(function(scheduleResult) {
            var schedule = new DataModel.Schedule(scheduleResult);
            var collectorObject = {};

            // todo: get service_instance id and collector status (active)
            collectorObject.data_series_input = dataSeriesResult.id;
            collectorObject.data_series_output = dataSeriesResultOutput.id;
            collectorObject.service_instance_id = serviceObject.id;
            collectorObject.schedule_id = scheduleResult.id;
            collectorObject.active = active;
            collectorObject.collector_type = 1;
            collectorObject.schedule_id = scheduleResult.id;

            self.addCollector(collectorObject, filterObject).then(function(collectorResult) {
              var collector = new DataModel.Collector(collectorResult);
              // var input_output_map = [];

              var inputOutputArray = [];

              for(var i = 0; i < dataSeriesResult.dataSets.length; ++i) {
                var inputDataSet = dataSeriesResult.dataSets[i];
                var outputDataSet;
                if (dataSeriesResultOutput.dataSets.length == 1)
                  outputDataSet = dataSeriesResultOutput.dataSets[0];
                else
                  outputDataSet = dataSeriesResultOutput.dataSets[i];

                inputOutputArray.push({
                  collector_id: collectorResult.id,
                  input_dataset: inputDataSet.id,
                  output_dataset: outputDataSet.id
                })
              }

              models.db['CollectorInputOutput'].bulkCreate(inputOutputArray).then(function(bulkInputOutputResult) {
                var input_output_map = [];
                bulkInputOutputResult.forEach(function(bulkResult) {
                  input_output_map.push({
                    input: bulkResult.input_dataset,
                    output: bulkResult.output_dataset
                  })
                });
                collector.input_output_map = input_output_map;
                collector.schedule = schedule.toObject();

                // checking for intersection
                if (!intersectionArray) {
                  resolve({
                    collector: collector,
                    input: dataSeriesResult,
                    output:dataSeriesResultOutput,
                    schedule: schedule
                  });
                } else {
                  intersectionArray.forEach(function(intersect) {
                    intersect.collector_id = collector.id;
                  });
                  models.db['Intersection'].bulkCreate(intersectionArray, {returning: true}).then(function(bulkIntersectionResult) {
                    collector.setIntersection(bulkIntersectionResult);
                    resolve({
                      collector: collector,
                      input: dataSeriesResult,
                      output:dataSeriesResultOutput,
                      schedule: schedule,
                      intersection: bulkIntersectionResult
                    });
                  }).catch(function(err) {
                    Utils.rollbackPromises([
                      self.removeDataSerie(dataSeriesResult),
                      self.removeDataSerie(dataSeriesResultOutput),
                      self.removeSchedule({id: scheduleResult.id})
                    ], new exceptions.AnalysisError("Could not save data series"), reject);
                  })
                }
              }).catch(function(err) {
                Utils.rollbackPromises([
                  self.removeDataSerie(dataSeriesResult),
                  self.removeDataSerie(dataSeriesResultOutput),
                  self.removeSchedule({id: scheduleResult.id})
                ], err, reject);
              })

            }).catch(function(err) {
              // rollback schedule
              console.log("rollback schedule")
              Utils.rollbackPromises([
                self.removeSchedule(scheduleResult),
                self.removeDataSerie(dataSeriesResult),
                self.removeDataSerie(dataSeriesResultOutput)
              ], err, reject);
            });
          }).catch(function(err) {
            // rollback dataseries
            console.log("rollback dataseries in out");
            Utils.rollbackPromises([
              self.removeDataSerie(dataSeriesResultOutput),
              self.removeDataSerie(dataSeriesResult)
            ], err, reject);
          });
        }).catch(function(err) {
          console.log("rollback dataseries");
          Utils.rollbackPromises([self.removeDataSerie(dataSeriesResult)], err, reject);
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
        resolve(new DataModel.Schedule(schedule.get()));
      }).catch(function(err) {
        // todo: improve error message
        reject(new exceptions.ScheduleError("Could not save schedule. ", err));
      });
    });
  },

  updateSchedule: function(scheduleId, scheduleObject) {
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db['Schedule'].update(scheduleObject, {
        fields: ['schedule', 'schedule_time', 'schedule_unit', 'frequency_unit', 'frequency'],
        where: {
          id: scheduleId
        }
      }).then(function() {
        resolve();
      }).catch(function(err) {
        reject(new exceptions.ScheduleError("Could not update schedule " + err.toString()));
      })
    });
  },

  removeSchedule: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.Schedule.destroy({where: {id: restriction.id}}).then(function() {
        resolve();
      }).catch(function(err) {
        console.log(err);
        reject(new exceptions.ScheduleError("Could not remove schedule " + err.message));
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
      self.listCollectors(restriction || {}).then(function(collectors) {
        if (collectors.length == 1)
          models.db['Schedule'].findOne({id: collectors[0].schedule_id}).then(function(scheduleResult) {
            resolve(scheduleResult.get());
          }).catch(function(err) {
            console.log(err);
            reject(new exceptions.ScheduleError("Could not find schedule " + err.message));
          });
        else {
          reject(new exceptions.ScheduleError("Could not find schedule with a collector associated"));
        }
      }).catch(function(err) {
        reject(err);
      })
    });
  },

  addCollector: function(collectorObject, filterObject) {
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.Collector.create(collectorObject).then(function(collectorResult) {
        if (_.isEmpty(filterObject))
          return resolve(collectorResult.get());

        if (_.isEmpty(filterObject.date)) {
          return resolve(collectorResult.get())
        } else {
          filterObject.collector_id = collectorResult.id;

          self.addFilter(filterObject).then(function(filterResult) {
            var output = Utils.clone(collectorResult.get());
            output.filter = filterResult;

            resolve(output);
          }).catch(function(err) {
            console.log(err);
            reject(new exceptions.CollectorError("Could not save collector: ", err));
          })
        }
      }).catch(function(err) {
        console.log(err);
        reject(new exceptions.CollectorError("Could not save collector: ", err));
      })
    });
  },

  updateCollectors: function(restriction, values, extra) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var options = Object.assign({
        where: restriction
      }, extra instanceof Object ? extra : {});

      models.db['Collector'].update(values, options).then(function() {
        resolve();
      }).catch(function(err) {
        console.log(err);
        reject(new exceptions.CollectorError("Could not update collector" + err.message));
      })

    });
  },

  updateCollector: function(collectorId, collectorObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var fields = [
        'service_instance_id',
        'data_series_input',
        'data_series_output',
        'schedule_id',
        'active',
        'collector_type'
      ]

      self.updateCollectors({id: collectorId}, collectorObject, {fields: fields}).then(function() {
        resolve();
      }).catch(function(err) {
        reject(err);
      });
    });
  },

  listCollectorInputOutput: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db['CollectorInputOutput'].findAll({where: restriction || {}}).then(function(inputOutputResult) {
        var output = [];
        inputOutputResult.forEach(function(element) {
          output.push(element.get());
        })
        resolve(output);
      }).catch(function(err) {
        reject(err);
      })
    })
  },

  listCollectors: function(restriction, projectId) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db['Collector'].findAll({
        where: restriction,
        include: [
          models.db.Schedule,
          models.db['CollectorInputOutput'],
          {
            model: models.db['Filter'],
            required: false,
            attributes: { include: [[connection.fn('ST_AsText', connection.col('region')), 'region_wkt']] }
          },
          {
            model: models.db['Intersection'],
            required: false
          }
        ]
      }).then(function(collectorsResult) {
        var output = [];

        var promises = [];
        collectorsResult.forEach(function(collector) {
          promises.push(self.getDataSeries({id: collector.data_series_output}));
        });

        Promise.all(promises).then(function(dataSeriesArray) {
          dataSeriesArray.forEach(function(dataSeries) {
            collectorsResult.some(function(collector) {
              if (collector.data_series_output === dataSeries.id) {
                var collectorInstance = new DataModel.Collector(collector.get());
                collectorInstance.dataSeriesOutput = dataSeries;
                output.push(collectorInstance);

                return true;
              }
            })
          })

          resolve(output);
        }).catch(function(err) {
          console.log(err);
          reject(new exceptions.CollectorError("Could not retrieve collector data series output: " + err.toString()));
        })

      }).catch(function(err) {
        console.log(err);
        reject(new exceptions.CollectorError("Could not retrieve collector: " + err.message));
      });
    });
  },

  getCollector: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var restrictionOutput = {};
      if (restriction.output) {
        Object.assign(restrictionOutput, restriction.output);
        delete restriction.output;
      }

      models.db.Collector.findOne({
        where: restriction,
        include: [
          {
            model: models.db['Schedule']
          },
          {
            model: models.db['DataSeries'],
            where: restrictionOutput
          },
          {
            model: models.db['CollectorInputOutput']
          },
          {
            model: models.db['Filter'],
            required: false,
            attributes: { include: [[connection.fn('ST_AsText', connection.col('region')), 'region_wkt']] }
          },
          {
            model: models.db['Intersection'],
            required: false
          }
        ]
      }).then(function(collectorResult) {
        if (collectorResult) {
          var collectorInstance = new DataModel.Collector(collectorResult.get());

          self.getDataSeries({id: collectorResult.data_series_output}).then(function(dataSeries) {
            collectorInstance.dataSeriesOutput = dataSeries;
            resolve(collectorInstance);
          }).catch(function(err) {
            console.log("Retrieved null while getting collector");
            reject(new exceptions.CollectorError("Could not find collector. "));
          })
        } else {
          console.log("Retrieved null while getting collector");
          reject(new exceptions.CollectorError("Could not find collector. "));
        }
      }).catch(function(err) {
        console.log(err);
        reject(new exceptions.CollectorError("Could not find collector. " + err.message));
      });
    });
  },

  addIntersection: function(intersectionArray) {
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.Intersection.bulkCreate(intersectionArray, {returning: true}).then(function(intesectionList) {
        var output = [];
        intesectionList.forEach(function(element) {
          output.push(new DataModel.Intersection(element.get()));
        })
        resolve(output);
      }).catch(function(err) {
        reject(new Error("Could not save intersection. " + err.toString()));
      })
    });
  },

  updateIntersection: function(intersectionId, intersectionObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.Intersection.update(intersectionObject, {
        fields: ['attribute'],
        where: {
          id: intersectionId
        }
      }).then(function() {
        resolve();
      }).catch(function(err) {
        reject(new Error("Could not update intersection " + err.toString()));
      })
    });
  },

  updateIntersections: function(intersectionIds, intersectionList) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var promises = [];
      intersectionList.forEach(function(element, index) {
        promises.push(self.updateIntersection(intersectionsIds[index], element));
      })
      Promise.all(promises).then(function() {
        resolve();
      }).catch(function(err) {
        reject(err);
      })
    })
  },

  listIntersections: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.Intersection.findAll(restriction).then(function(intersectionResult) {
        var output = [];
        intersectionResult.forEach(function(element) {
          output.push(element.get());
        })
        resolve(output);
      }).catch(function(err) {
        reject(new Error("Could not retrieve intersection " + err.toString()));
      })
    });
  },

  removeIntersection: function(restriction) {
    return new Promise(function(resolve, reject) {
      models.db.Intersection.destroy({where: restriction}).then(function() {
        resolve();
      }).catch(function(err) {
        reject(new Error("Could not remove intersection " + err.toString()));
      })
    });
  },

  addFilter: function(filterObject) {
    var self = this;

    return new Promise(function(resolve, reject) {
      var filterValues = {collector_id: filterObject.collector_id};

      Object.assign(filterValues, _processFilter(filterObject));

      // checking filter
      models.db.Filter.create(filterValues).then(function(filter) {
        if (filter.region) {
          self.getWKT(filter.region).then(function(geom) {
            var filter = new DataModel.Filter(filter.get());
            filter.region_wkt = geom;
            resolve(filter);
          }).catch(function(err) {
            reject(err);
          })
        } else {
          resolve(new DataModel.Filter(filter.get()));
        }
      }).catch(function(err) {
        // todo: improve error message
        reject(new Error("Could not save filter. ", err));
      });
    });
  },

  updateFilter: function(filterId, filterObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var filterValues = _processFilter(filterObject);
      models.db.Filter.update(filterValues, {
        fields: ['frequency', 'frequency_unit', 'discard_before', 'discard_after', 'region', 'by_value'],
        where: {
          id: filterId
        }
      }).then(function() {
        resolve();
      }).catch(function(err) {
        console.log(err);
        reject(new Error("Could not update filter " + err.toString()))
      })
    });
  },

  addAnalysis: function(analysisObject, scheduleObject, dataSeriesObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var _rollbackDataSeries = function(err, instance) {
        self.removeDataSerie({id: instance.id}).then(function() {
          console.log(err);
          reject(new exceptions.AnalysisError("Could not save analysis: " + err.message));
        }).catch(function(err) {
          console.log("Error rollback data series analysis: ", err);
          reject(err);
        });
      };

      // adding dataseries_output
      self.addDataSeries(dataSeriesObject, {
        data_series_id: analysisObject.data_series_id,
        type: analysisObject.type
      }).then(function(dataSeriesResult) {

        self.addSchedule(scheduleObject).then(function(scheduleResult) {
          // adding analysis
          // todo: make it as factory: AnalysisGrid, Analysis...
          analysisObject.dataset_output = dataSeriesResult.dataSets[0].id;
          analysisObject.schedule_id = scheduleResult.id;

          var scopeAnalysisObject = analysisObject;
          models.db["Analysis"].create(analysisObject).then(function(analysisResult) {
            analysisResult.getScriptLanguage().then(function(scriptLanguageResult) {
              var analysisDataSeriesArray = Utils.clone(scopeAnalysisObject.analysisDataSeries);

              // making analysis metadata
              var analysisMetadata = [];
              for(var key in analysisObject.metadata) {
                if (analysisObject.metadata.hasOwnProperty(key)) {
                  analysisMetadata.push({
                    analysis_id: analysisResult.id,
                    key: key,
                    value: analysisObject.metadata[key]
                  })
                }
              }

              models.db['AnalysisMetadata'].bulkCreate(analysisMetadata).then(function(bulkAnalysisMetadata) {
                var analysisMetadataOutput = {};
                bulkAnalysisMetadata.forEach(function(bulkMetadata) {
                  analysisMetadataOutput[bulkMetadata.key] = bulkMetadata.value;
                });

                var promises = [];

                analysisDataSeriesArray.forEach(function(analysisDS) {
                  analysisDS.analysis_id = analysisResult.id;

                  var metadata = [];
                  for(var key in analysisDS.metadata) {
                    if (analysisDS.metadata.hasOwnProperty(key)) {
                      metadata.push({
                        key: key,
                        value: analysisDS.metadata[key]
                      });
                    }
                  }
                  delete analysisDS.metadata;
                  analysisDS.AnalysisDataSeriesMetadata = metadata;

                  promises.push(models.db['AnalysisDataSeries'].create(analysisDS, {include: [models.db['AnalysisDataSeriesMetadata']]}));
                });

                var analysisInstance = new DataModel.Analysis(analysisResult);
                analysisInstance.setScriptLanguage(scriptLanguageResult);
                analysisInstance.setSchedule(scheduleResult);
                analysisInstance.setMetadata(analysisMetadataOutput);
                analysisInstance.setDataSeries(dataSeriesResult);

                analysisResult.getAnalysisType().then(function(analysisTypeResult) {
                  analysisInstance.type = analysisTypeResult.get();
                  Promise.all(promises).then(function(results) {
                    results.forEach(function(result) {
                      var analysisDataSeries = new DataModel.AnalysisDataSeries(result.get());
                      var analysisDataSeriesMetadata = {};

                      result.AnalysisDataSeriesMetadata.forEach(function(meta) {
                        var data = meta.get();
                        analysisDataSeriesMetadata[data.key] = data.value;
                      });

                      analysisDataSeries.metadata = analysisDataSeriesMetadata;

                      analysisInstance.addAnalysisDataSeries(analysisDataSeries);
                    });

                    // setting metadata

                    resolve(analysisInstance);
                  }).catch(function(err) {
                    console.log(err);
                    // rollback analysis data series
                    Utils.rollbackPromises([
                      self.removeDataSerie({id: dataSeriesResult.id}),
                      self.removeSchedule({id: scheduleResult.id}),
                    ], new exceptions.AnalysisError("Could not save analysis data series"), reject);
                  });
                }).catch(function(err) {
                  // rollback analysis metadata
                  Utils.rollbackPromises([
                    self.removeDataSerie({id: dataSeriesResult.id}),
                    self.removeSchedule({id: scheduleResult.id}),
                  ], new exceptions.AnalysisError("Could not save analysis while retrieving analysis type " + err.message), reject);
                })

              }).catch(function(err) {
                // rollback analysis metadata
                Utils.rollbackPromises([
                  self.removeDataSerie({id: dataSeriesResult.id}),
                  self.removeSchedule({id: scheduleResult.id}),
                ], new exceptions.AnalysisError("Could not save analysis metadata " + err.message), reject);
              })
            }).catch(function(err) {
              // rollback data series
              Utils.rollbackPromises([
                self.removeDataSerie({id: dataSeriesResult.id}),
                self.removeSchedule({id: scheduleResult.id}),
              ], new exceptions.AnalysisError("Could not save retrieve analysis script language"), reject);
            })
          }).catch(function(err) {
            // analysis
            // rollback data series
            Utils.rollbackPromises([
              self.removeDataSerie({id: dataSeriesResult.id}),
              self.removeSchedule({id: scheduleResult.id}),
            ], new exceptions.AnalysisError("Could not save analysis"), reject);
          })
        }).catch(function(err) {
          // schedule
          Utils.rollbackPromises([self.removeDataSerie({id: dataSeriesResult.id})], new exceptions.AnalysisError("Could not save analysis schedule"), reject);
        });
      }).catch(function(err) {
        // dataseries
        console.log(err);
        reject(err);
      });
    });
  },

  updateAnalysis: function(analysisId, analysisObject, scheduleObject) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getAnalysis({id: analysisId}).then(function(analysisInstance) {
        models.db['Analysis'].update(analysisObject, {
          fields: ['name', 'description', 'instance_id', 'script'],
          where: {
            id: analysisId
          }
        }).then(function() {
          // updating analysis dataSeries
          var promises = [];

          analysisInstance.analysis_dataseries_list.forEach(function(analysisDS) {
            promises.push(models.db['AnalysisDataSeries'].update(
              {alias: analysisDS.alias},
              {
                fields: ['alias'],
                where: {
                  id: analysisDS.id
                }
              }
            ));
          })

          Promise.all(promises).then(function() {
            // updating schedule
            self.updateSchedule(analysisInstance.schedule.id, scheduleObject).then(function() {
              resolve();
            }).catch(function(err) {
              reject(err);
            });

          }).catch(function(err) {
            reject(new exceptions.AnalysisError("Could not update analysis data series " + err.toString()));
          });

        }).catch(function(err) {
          reject(new exceptions.AnalysisError("Could not update analysis. " + err.toString()));
        })
      }).catch(function(err) {
        reject(err);
      })
    })
  },

  listAnalyses: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var _reject = function(err) {
        console.log(err);
        reject(err);
      };
      models.db['Analysis'].findAll({
        include: [
          {
            model: models.db['AnalysisDataSeries'],
            include: [
              {
                model: models.db['AnalysisDataSeriesMetadata'],
                required: false
              }
            ]
          },
          models.db['AnalysisMetadata'],
          models.db['ScriptLanguage'],
          models.db['AnalysisType'],
          models.db['Schedule']
        ]
      }).then(function(analysesResult) {
        var output = [];
        var promises = [];

        analysesResult.forEach(function(analysis) {
          promises.push(self.getDataSet({id: analysis.dataset_output}));
        });

        Promise.all(promises).then(function(dataSets) {
          promises = [];

          dataSets.forEach(function(dataSet) {
            promises.push(self.getDataSeries({id: dataSet.data_series_id}));
          });

          Promise.all(promises).then(function(dataSeriesList) {
            analysesResult.forEach(function(analysis) {
              var analysisObject = new DataModel.Analysis(analysis.get());
              dataSets.some(function(dataSet) {
                return dataSeriesList.some(function(dataSeries) {
                  if (dataSet.data_series_id === dataSeries.id) {
                    analysisObject.setDataSeries(dataSeries);
                    return true;
                  }
                })
              })

              analysis.AnalysisDataSeries.forEach(function(analysisDataSeries) {
                analysisObject.addAnalysisDataSeries(new DataModel.AnalysisDataSeries(analysisDataSeries.get()));
              });

              output.push(analysisObject);
            });

            resolve(output);
          }).catch(_reject);
        }).catch(_reject);
      }).catch(_reject);
    });
  },

  getAnalysis: function(restriction) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var restrict = Object.assign({}, restriction || {});
      var dataSeriesRestriction = {};
      if (restrict && restrict.dataSeries) {
        dataSeriesRestriction = restrict.dataSeries;
        delete restrict.dataSeries;
      }
      models.db['Analysis'].findOne({
        where: restrict,
        include: [
          {
            model: models.db['AnalysisDataSeries'],
            include: [
              {
                model: models.db['AnalysisDataSeriesMetadata'],
                required: false
              }
            ]
          },
          models.db['AnalysisMetadata'],
          models.db['ScriptLanguage'],
          models.db['AnalysisType'],
          models.db['Schedule'],
          {
            model: models.db['DataSet'],
            include: [
              {
                model: models.db['DataSeries'],
                where: dataSeriesRestriction
              }
            ]
          }
        ]
      }).then(function(analysisResult) {
        var analysisInstance = new DataModel.Analysis(analysisResult.get());

        self.getDataSet({id: analysisResult.dataset_output}).then(function(analysisOutputDataSet) {
          self.getDataSeries({id: analysisOutputDataSet.data_series_id}).then(function(analysisOutputDataSeries) {
            analysisInstance.setDataSeries(analysisOutputDataSeries);
            analysisResult.AnalysisDataSeries.forEach(function(analysisDataSeries) {
              var ds = getItemByParam(self.data.dataSeries, {id: analysisDataSeries.data_series_id});
              var analysisDsMeta = new DataModel.AnalysisDataSeries(analysisDataSeries.get());
              analysisDsMeta.setDataSeries(ds);
              analysisInstance.addAnalysisDataSeries(analysisDsMeta);
            });

            resolve(analysisInstance);
          }).catch(function(err) {
            reject(err);
          })
        }).catch(function(err) {
          reject(err);
        })
      }).catch(function(err) {
        console.log(err);
        reject(new exceptions.AnalysisError("Could not retrieve Analysis " + err.message));
      });
    });
  },

  /**
   * It removes Analysis from param. It should be an object containing either id identifier or name identifier.
   *
   * @param {Object} analysisParam - An object containing Analysis identifier to get it.
   * @param {bool} cascade - A bool value to delete on cascade
   * @return {Promise} - a 'bluebird' module with Analysis instance or error callback
   */
  removeAnalysis: function(analysisParam, cascade) {
    var self = this;
    return new Promise(function(resolve, reject) {
      if(!cascade)
        cascade = false;

      self.getAnalysis({id: analysisParam.id}).then(function(analysisResult) {
        models.db.Analysis.destroy({where: {id: analysisParam.id}}).then(function() {
          self.removeDataSerie({id: analysisResult.dataSeries.id}).then(function() {
            self.removeSchedule({id: analysisResult.schedule.id}).then(function() {
              resolve();
            }).catch(function(err) {
              console.log("Could not remove analysis schedule ", err);
              reject(err);
            })
          }).catch(function(err) {
            console.log("Could not remove output data series ", err);
            reject(err);
          })
        }).catch(function(err) {
          console.log(err);
          reject(new exceptions.AnalysisError("Could not remove Analysis with a collector associated", err));
        });
      }).catch(function(err) {
        console.log(err);
        reject(err);
      })
    });
  }

};

module.exports = DataManager;
