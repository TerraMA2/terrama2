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

var Application = require("./Application");
var modelsFn = require("../models");
var exceptions = require('./Exceptions');
var Promise = require('bluebird');
var Utils = require('./Utils');
var _ = require('lodash');
var Enums = require('./Enums');
var Database = require('../config/Database');
var orm = null;
var fs = require('fs');
var path = require('path');
var logger = require("./Logger");
var Filters = require("./filters");

// data model
var DataModel = require('./data-model');

// Available DataSeriesType
var DataSeriesType = Enums.DataSeriesType;

function _processFilter(filterObject) {
  var filterValues = Object.assign({}, filterObject);
  // checking filter by date
  if (filterObject.hasOwnProperty('date') && !_.isEmpty(filterObject.date)) {
    if (filterObject.date.beforeDate) {
      filterValues.discard_before = new Date(filterObject.date.beforeDate);
    }
    if (filterObject.date.afterDate) {
      filterValues.discard_after = new Date(filterObject.date.afterDate);
    }
  }
  if (filterObject.hasOwnProperty('area')){
    if (filterObject.area.hasOwnProperty('crop_raster')){
      filterValues.crop_raster = filterObject.area.crop_raster;
    }
  } else if (filterObject.hasOwnProperty('crop_raster')){
    filterValues.crop_raster = filterObject.crop_raster;
  } else {
    filterValues.crop_raster = false;
  }

  return filterValues;
}


var models = null;

/**
 * Controller of the system index.
 * @class DataManager
 *
 * @property {object} data - Object for storing model values, such DataProviders, DataSeries and Projects.
 * @property {Boolean} isLoaded - A flag value to determines if DataManager has been loaded before.
 */
var DataManager = module.exports = {
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
  DataModel: DataModel,
  /**
   * It defines a orm instance
   * @type {Sequelize.Transaction}
   */
  orm: orm,

  /**
   * It initializes DataManager, loading models and database synchronization
   * @param {function} dbConfigPath - Optional database config file path
   * @param {function} callback - A callback function for waiting async operation
   */
  init: function(dbConfigPath, callback) {
    var self = this;
    logger.info("Initializing database...");

    return Database.init(dbConfigPath).then(function(dbORM) {
      logger.info("Database loaded.");
      self.orm = orm = dbORM;

      var dbConfig = Application.getContextConfig().db;

      models = modelsFn();
      models.load(self.orm);

      var fn = function() {
        var inserts = [];

        // default users
        var salt = models.db.User.generateSalt();

        // admin
        inserts.push(models.db.User.create({
          name: "Administrator",
          username: "admin",
          password: models.db.User.generateHash("admin", salt),
          salt: salt,
          cellphone: '14578942362',
          email: 'admin@terrama2.inpe.br',
          administrator: true,
          token: models.db.User.generateToken("admin@terrama2.inpe.bradmin")
        }));

        // services type
        inserts.push(models.db.ServiceType.create({id: Enums.ServiceType.COLLECTOR, name: "COLLECT"}));
        inserts.push(models.db.ServiceType.create({id: Enums.ServiceType.ANALYSIS, name: "ANALYSIS"}));
        inserts.push(models.db.ServiceType.create({id: Enums.ServiceType.VIEW, name: "VIEW"}));
        inserts.push(models.db.ServiceType.create({id: Enums.ServiceType.ALERT, name: "ALERT"}));

        // data provider type defaults
        inserts.push(self.addDataProviderType({id: 1, name: "FILE", description: "Desc File"}));
        inserts.push(self.addDataProviderType({id: 2, name: "FTP", description: "Desc FTP"}));
        inserts.push(self.addDataProviderType({id: 3, name: "HTTP", description: "Desc Http"}));
        inserts.push(self.addDataProviderType({id: 4, name: "POSTGIS", description: "Desc Postgis"}));
        //inserts.push(self.addDataProviderType({id: 5, name: "SFTP", description: "Desc SFTP"}));
        inserts.push(self.addDataProviderType({id: 6, name: "HTTPS", description: "Desc Https"}));

        var listServicesPromise = self.listServiceInstances({}).then(function(services){
          var servicesInsert = [];
          if (services.length == 0 ){

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
                database: dbConfig.database
              }
            };

            var analysisService = Object.assign({}, collectorService);
            analysisService.name = "Local Analysis";
            analysisService.description = "Local service for Analysis";
            analysisService.port = 6544;
            analysisService.service_type_id = Enums.ServiceType.ANALYSIS;

            var viewService = Object.assign({}, collectorService);
            viewService.name = "Local View";
            viewService.description = "Local service for View";
            viewService.port = 6545;
            viewService.service_type_id = Enums.ServiceType.VIEW;
            viewService.metadata = {
              maps_server: "http://admin:geoserver@localhost:8080/geoserver"
            };

            var alertService = Object.assign({}, collectorService);
            alertService.name = "Local Alert";
            alertService.description = "Local service for Alert";
            alertService.port = 6546;
            alertService.service_type_id = Enums.ServiceType.ALERT;
            alertService.metadata = { };

            servicesInsert.push(self.addServiceInstance(collectorService));
            servicesInsert.push(self.addServiceInstance(analysisService));
            servicesInsert.push(self.addServiceInstance(viewService));
            servicesInsert.push(self.addServiceInstance(alertService));
          }
          return Promise.all(servicesInsert);
        });
        inserts.push(listServicesPromise);


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
        inserts.push(models.db.DataSeriesType.create({name: DataSeriesType.GEOMETRIC_OBJECT, description: "Data Series Geometric object"}));

        // data formats semantics defaults
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.CSV, description: "CSV description"}));
        inserts.push(self.addDataFormat({name: DataSeriesType.OCCURRENCE, description: "Occurrence description"}));
        inserts.push(self.addDataFormat({name: DataSeriesType.GRID, description: "Grid Description"}));
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.POSTGIS, description: "POSTGIS description"}));
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.OGR, description: "Gdal ogr"}));
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.GDAL, description: "GDAL"}));
        inserts.push(self.addDataFormat({name: Enums.DataSeriesFormat.GRADS, description: "GRADS"}));

        // analysis type
        inserts.push(models.db.AnalysisType.create({id: Enums.AnalysisType.DCP, name: "Dcp", description: "Description Dcp"}));
        inserts.push(models.db.AnalysisType.create({id: Enums.AnalysisType.GRID, name: "Grid", description: "Description Grid"}));
        inserts.push(models.db.AnalysisType.create({id: Enums.AnalysisType.MONITORED, name: "Monitored Object", description: "Description Monitored"}));

        // analysis data series type
        inserts.push(models.db.AnalysisDataSeriesType.create({
          id: Enums.AnalysisDataSeriesType.DATASERIES_DCP_TYPE,
          name: "Dcp",
          description: "Description Dcp"
        }));
        inserts.push(models.db.AnalysisDataSeriesType.create({
          id: Enums.AnalysisDataSeriesType.DATASERIES_GRID_TYPE,
          name: "Grid",
          description: "Description Grid"
        }));

        inserts.push(models.db.AnalysisDataSeriesType.create({
          id: Enums.AnalysisDataSeriesType.DATASERIES_MONITORED_OBJECT_TYPE,
          name: "Monitored Object",
          description: "Description Monitored"
        }));

        inserts.push(models.db.AnalysisDataSeriesType.create({
          id: Enums.AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE,
          name: "Additional Data",
          description: "Description Additional Data"
        }));

        // area of interest
        inserts.push(models.db.AnalysisAreaOfInterestType.create({
          id: Enums.InterestAreaType.UNION.value,
          name: Enums.InterestAreaType.UNION.name,
          description: Enums.InterestAreaType.UNION.name
        }));
        inserts.push(models.db.AnalysisAreaOfInterestType.create({
          id: Enums.InterestAreaType.SAME_FROM_DATA_SERIES.value,
          name: Enums.InterestAreaType.SAME_FROM_DATA_SERIES.name,
          description: Enums.InterestAreaType.SAME_FROM_DATA_SERIES.name
        }));
        inserts.push(models.db.AnalysisAreaOfInterestType.create({
          id: Enums.InterestAreaType.CUSTOM.value,
          name: Enums.InterestAreaType.CUSTOM.name,
          description: Enums.InterestAreaType.CUSTOM.name
        }));

        // resolution type
        inserts.push(models.db.AnalysisResolutionType.create({
          id: Enums.ResolutionType.BIGGEST_GRID.value,
          name: Enums.ResolutionType.BIGGEST_GRID.name,
          description: Enums.ResolutionType.BIGGEST_GRID.name
        }));
        inserts.push(models.db.AnalysisResolutionType.create({
          id: Enums.ResolutionType.SMALLEST_GRID.value,
          name: Enums.ResolutionType.SMALLEST_GRID.name,
          description: Enums.ResolutionType.SMALLEST_GRID.name
        }));
        inserts.push(models.db.AnalysisResolutionType.create({
          id: Enums.ResolutionType.SAME_FROM_DATA_SERIES.value,
          name: Enums.ResolutionType.SAME_FROM_DATA_SERIES.name,
          description: Enums.ResolutionType.SAME_FROM_DATA_SERIES.name
        }));
        inserts.push(models.db.AnalysisResolutionType.create({
          id: Enums.ResolutionType.CUSTOM.value,
          name: Enums.ResolutionType.CUSTOM.name,
          description: Enums.ResolutionType.CUSTOM.name
        }));

        // Interpolation methods
        inserts.push(models.db.InterpolationMethod.create({
          id: Enums.InterpolationMethod.NEAREST_NEIGHBOR.value,
          name: Enums.InterpolationMethod.NEAREST_NEIGHBOR.name,
          description: Enums.InterpolationMethod.NEAREST_NEIGHBOR.name
        }));
        inserts.push(models.db.InterpolationMethod.create({
          id: Enums.InterpolationMethod.BI_LINEAR.value,
          name: Enums.InterpolationMethod.BI_LINEAR.name,
          description: Enums.InterpolationMethod.BI_LINEAR.name
        }));
        inserts.push(models.db.InterpolationMethod.create({
          id: Enums.InterpolationMethod.BI_CUBIC.value,
          name: Enums.InterpolationMethod.BI_CUBIC.name,
          description: Enums.InterpolationMethod.BI_CUBIC.name
        }));

        // script language supported
        inserts.push(models.db.ScriptLanguage.create({id: Enums.ScriptLanguage.PYTHON, name: "PYTHON"}));
        inserts.push(models.db.ScriptLanguage.create({id: Enums.ScriptLanguage.LUA, name: "LUA"}));

        // it will match each of semantics with providers
        return Promise.all(inserts)
          .catch(function(err) {
            logger.debug(err);
            return null;
          }).finally(function() {
            // semantics: temp code: TODO: fix
            var semanticsObject = Application.get("semantics");

            // storing semantics providers dependency
            var semanticsWithProviders = {};

            var promises = [];

            semanticsObject.forEach(function(semanticsElement) {
              semanticsWithProviders[semanticsElement.code] = semanticsElement.providers_type_list;
              promises.push(self.addDataSeriesSemantics({
                code: semanticsElement.code,
                data_format_name: semanticsElement.format,
                data_series_type_name: semanticsElement.type
              }, semanticsElement.providers_type_list, semanticsElement.metadata));
            });

            return Promise.all(promises)
              .then(function() {
                logger.debug("DB initialized successfully");
                return null;
              })
              .catch(function(err) {
                logger.debug(err);
                return null;
              })
              .finally(function() {
                return callback();
              });
          });
      };

      return orm.authenticate().then(function() {
        return orm.sync().then(function () {
          return fn();
        }).catch(function(err) {
          logger.debug(err);
          return fn();
        });
      }).catch(function(err) {
        callback(new Error("Could not initialize TerraMA2 due: " + err.message));
      });
    })
    .catch(function(err) {
      return callback(err);
    });
  },

  /**
   * It releases cached data from memory
   *
   * @returns {Promise}
   */
  unload: function() {
    var self = this;
    return new Promise(function(resolve) {
      self.data.dataProviders = [];
      self.data.dataSeries = [];
      self.data.dataSets = [];
      self.data.projects = [];

      self.isLoaded = false;

      return resolve();
    });
  },

  /**
   * It finalizes DataManager instance and Database connection
   *
   * @returns {Promise}
   */
  finalize: function() {
    var self = this;
    return new Promise(function(resolve) {
      self.unload().then(function() {
        resolve();

        Database.finalize();
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
      // helper for clean up DataManager and reject promise
      var _rejectClean = function(err) {
        clean();
        logger.error("CLEAN: ", err);
        return reject(err);
      };

      return models.db.Project.findAll({}).then(function(projects) {
        return models.db.User.findAll({}).then(function(users) {
          projects.forEach(function(project) {
            var projectObj = project.get();
            var userObj = null;

            users.forEach(function(user) {
              if(user.dataValues.id === projectObj.user_id) {
                userObj = user.get();
                return;
              }
            });

            projectObj.user_name = userObj.name;

            self.data.projects.push(projectObj);
          });

          return models.db.DataProvider.findAll({ include: [ models.db.DataProviderType, models.db.DataProviderOptions ] }).then(function(dataProviders) {
            dataProviders.forEach(function(dataProvider) {
              self.data.dataProviders.push(new DataModel.DataProvider(dataProvider));
            });
            // find all data series, providers
            return models.db.DataSeries.findAll({
              include: [
                {
                  model: models.db.DataProvider,
                  include: [models.db.DataProviderType]
                },
                models.db.DataSeriesSemantics,
                {
                  model: models.db.DataSet,
                  include: [
                    {
                      model: models.db.DataSetDcp,
                      attributes: [
                        // retrieving GeoJSON. Its is important because Sequelize
                        // orm does not retrieve SRID even geometry has. The "2"
                        // in arguments is to retrieve entire representation.
                        // It retrieves as string. Once GeoJSON retrieved,
                        // you must parse it. "JSON.parse(geoJsonStr)"
                        [orm.fn('ST_AsGeoJSON', orm.col('position'), 15, 2), 'position'],
                        // EWKT representation
                        [orm.fn('ST_AsEwkt', orm.col('position')), 'positionWkt']
                      ],
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
                    {
                      model: models.db.DataSetGrid,
                      required: false
                    },
                    models.db.DataSetFormat
                  ]
                }
              ]
            }).then(function(dataSeries) {
              var semanticsList = Application.get("semantics");
              dataSeries.forEach(function(dSeries) {
                var provider = new DataModel.DataProvider(dSeries.DataProvider.get());

                var builtDataSeries = new DataModel.DataSeries(dSeries.get());
                builtDataSeries.setDataProvider(provider);

                builtDataSeries.dataSets.forEach(function(dSet) {
                  self.data.dataSets.push(dSet);
                });
                var semantic = semanticsList.find(function(dSeriesSemantic){
                  return dSeries.DataSeriesSemantic.code == dSeriesSemantic.code;
                });
                Object.assign(builtDataSeries.data_series_semantics, semantic);
                builtDataSeries.semantics = builtDataSeries.data_series_semantics;
                self.data.dataSeries.push(builtDataSeries);
              });

              self.isLoaded = true;
              return resolve();

            }).catch(_rejectClean);
          }).catch(_rejectClean);
        }).catch(_rejectClean);
      }).catch(_rejectClean);
    });
  },

  /**
   * It retrieves a WKT geometry from a valid GeoJSON
   * @param {Object} geoJSONObject - A GeoJSON object to be converted to WKT format
   * @return {string} WKT representation
   */
  getWKT: function(geoJSONObject) {
    return new Promise(function(resolve, reject) {
      models.db.sequelize.query("SELECT ST_AsEwkt(ST_GeomFromGeoJson('" + JSON.stringify(geoJSONObject) + "')) as geom").then(function(wktGeom) {
        // it retrieves an array with data result (array) and query executed.
        // if data result is empty or greater than 1, its not allowed.
        if (wktGeom[0].length !== 1) { reject(new exceptions.DataSetError("Invalid wkt retrieved from GeoJSON.")); }
        else {
          resolve(wktGeom[0][0].geom);
        }
      }).catch(function(err) {
        reject(err);
      });
    });
  },

  /**
   * It saves Project in database and storage it in memory
   * @param {Object} projectObject - An object containing project values to be saved.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module. The callback is either a {Project} data values or error
   */
  addProject: function(projectObject, options) {
    var self = this;
    return new Promise(function(resolve, reject){
      models.db.Project.create(projectObject, Utils.extend({}, options)).then(function(project){
        models.db.User.findAll({}).then(function(users) {
          var projectObj = project.get();
          var userObj = null;

          users.forEach(function(user) {
            if(user.dataValues.id === projectObj.user_id) {
              userObj = user.get();
              return;
            }
          });

          projectObj.user_name = userObj.name;

          self.data.projects.push(projectObj);
          return resolve(Utils.clone(projectObj));
        });
      }).catch(function(e) {
        var message = "Could not save project: ";
        if (e.errors) { message += e.errors[0].message; }
        return reject(new exceptions.ProjectError(message));
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
      var project = Utils.find(self.data.projects, projectParam);
      if (project) { resolve(Utils.clone(project)); }
      else { reject(new exceptions.ProjectError("Project not found")); }
    });
  },

  /**
   * It updates a project from given object values.
   *
   * @param {Object} projectObject - an javascript object containing project values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataFormat>} - a 'bluebird' module with DataFormat instance or error callback
   */
  updateProject: function(projectObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getProject({id: projectObject.id}).then(function(project) {
        var fieldsToUpdate = ["name", "description", "version", "protected"];
        // Add user if the project dont have one
        if (!project.user_id){
          fieldsToUpdate.push("user_id");
        }

        return models.db.Project.update(projectObject, Utils.extend({
          fields: fieldsToUpdate,
          where: {
            id: project.id
          }
        }, options)).then(function() {
          var projectItem = Utils.find(self.data.projects, {id: projectObject.id});
          projectItem.name = projectObject.name;
          projectItem.description = projectObject.description;
          projectItem.version = projectObject.version;
          projectItem.protected = projectObject.protected;
          projectItem.user_id = projectObject.user_id;

          return resolve(Utils.clone(projectItem));
        }).catch(function(err) {
          return reject(new exceptions.ProjectError("Could update project " + err.toString()));
        });
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It removes project of database from given restriction. **Note** If there is no restriction specified, it will remove all rows of model
   *
   * @param {Object} restriction - A query restriction object
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  removeProject: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getProject(restriction).then(function(projectResult) {
        return self.listCollectors({}, options).then(function(collectors) {
          return self.listAnalysis({project_id: projectResult.id}, options).then(function(analysisList) {
            return self.listViews({project_id: projectResult.id}).then(function(views) {
              var scheduleIds = [];
              collectors.forEach(function(collector) {
                if (collector.dataSeriesOutput && collector.dataSeriesOutput.dataProvider.project_id === projectResult.id) {
                  scheduleIds.push(collector.schedule.id);
                }
              });

              var scheduleAnalysis = analysisList.map(function(analysis) {
                return analysis.schedule.id;
              });

              var scheduleViews = [];
              for(var i = 0; i < views.length; ++i) {
                var view = views[i];
                if (view.schedule && view.schedule.id) {
                  scheduleViews.push(view.schedule.id);
                }
              }

              var scheduleIdentifiers = Utils.concat(scheduleIds, scheduleAnalysis, scheduleViews);

              return self.removeSchedule({id: {$in: scheduleIdentifiers}}, options);
            });
          }).finally(function() {
            return models.db.Project.destroy({
              where: {
                id: projectResult.id
              }
            }).then(function() {
              var project = Utils.remove(self.data.projects, {id: projectResult.id});
              // remove children from memory
              Utils.removeAll(self.data.dataProviders, {project_id: project.id});
              Utils.removeAll(self.data.dataSeries, {dataProvider: {project_id: project.id}});
              return resolve();
            }).catch(function(err) {
              logger.error("Remove Project: ", err);
              return reject(new exceptions.ProjectError("Could not remove project with data provider associated"));
            });
          });
        });
      }).catch(function(err) {
        return reject(err);
      });
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
   * It add a new TerraMA2 user instance
   * @param {Object} userObject - A javascript object with user values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} a bluebird promise
   */
  addUser: function(userObject, options) {
    return new Promise(function(resolve, reject) {
      var salt = models.db.User.generateSalt();
      userObject.salt = salt;
      userObject.administrator = userObject.administrator !== undefined && userObject.administrator === true;
      userObject.token = models.db.User.generateToken(userObject.email + userObject.password);
      userObject.password = models.db.User.generateHash(userObject.password, salt);

      return models.db.User.create(userObject, options)
        .then(function(newUser) {
          return resolve(newUser.get());
        })
        .catch(function(err) {
          return reject(new exceptions.UserError(Utils.format("Could not save user due %s", err.toString())));
        });
    });
  },
  /**
   * It updates a TerraMA2 user instance
   * @param {Object} restriction - A javascript object to identify a user
   * @param {Object} userObject - A javascript object with user values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} a bluebird promise
   */
  updateUser: function(restriction, userObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getUser(restriction, options).then(function(user) {
        if (!userObject.password) {
          userObject.password = user.password;
        } else {
          var salt = user.salt;
          userObject.password = models.db.User.generateHash(userObject.password, salt);
        }
        return models.db.User.update(userObject, Utils.extend({
          fields: ['name', 'cellphone', 'administrator', 'email', 'password'],
          where: restriction || {}
        }, options)).then(function() {
          return resolve();
        }).catch(function(err) {
          logger.error(err);
          return reject(new exceptions.UserError("Could not update user.", err.errors));
        });
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It retrieves all users in database filtering from given restriction
   *
   * @param {Object} restriction - A javascript object with restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<Array<User>>} a bluebird module with users
   */
  listUsers: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.User.findAll(Utils.extend({where: restriction || {} }, options)).then(function(users) {
        return resolve(users.map(function(userInstance) { return userInstance.get(); }));
      }).catch(function(err) {
        return reject(new exceptions.UserError("Could not update user.", err.errors||[]));
      });
    });
  },

  /**
   * It retrieves a user from given restriction
   *
   * @param {Object} restriction - A javascript object with query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<User>} a bluebird module with user sequelize instance
   */
  getUser: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.User.findOne(Utils.extend({where: restriction || {}}, options)).then(function(user) {
        if (user === null) {
          return reject(new exceptions.UserError("Could not get user.", []));
        }
        return resolve(user);
      }).catch(function(err) {
        return reject(new exceptions.UserError("Could not update user.", err.errors||[]));
      });
    });
  },

  removeUser: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      return models.db.User.destroy(Utils.extend({where: restriction}, options))
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(new exceptions.UserError("Could not remove user " + err.toString()));
        });
    });
  },

  /**
   * Adds a new monitor state.
   * @param {Object} stateObject - A javascript object with state values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} a bluebird promise
   */
  addMonitorState: function(stateObject, options) {
    return new Promise(function(resolve, reject) {
      return models.db.MonitorState.create(stateObject, options).then(function(newState) {
        return resolve(newState.get());
      }).catch(function(err) {
        return reject(new Error(Utils.format("Could not save state due %s", err.toString())));
      });
    });
  },

  /**
   * Updates a monitor state.
   * @param {Object} restriction - A javascript object to identify a state
   * @param {Object} stateObject - A javascript object with state values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} a bluebird promise
   */
  updateMonitorState: function(restriction, stateObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.MonitorState.update(stateObject, Utils.extend({
        fields: ['name', 'state'],
        where: restriction || {}
      }, options)).then(function() {
        return resolve();
      }).catch(function(err) {
        logger.error(err);
        return reject(new Error("Could not update state.", err.errors));
      });
    });
  },

  /**
   * Retrieves all monitor states in database filtering from given restriction.
   *
   * @param {Object} restriction - A javascript object with restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<Array<MonitorState>>} a bluebird module with states
   */
  listMonitorStates: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.MonitorState.findAll(Utils.extend({ where: restriction || {} }, options)).then(function(states) {
        return resolve(
          states.map(function(stateInstance) {
            return stateInstance.get();
          })
        );
      }).catch(function(err) {
        return reject(new Error("Could not list states.", err.errors || []));
      });
    });
  },

  /**
   * Retrieves a monitor state from given restriction
   *
   * @param {Object} restriction - A javascript object with query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<MonitorState>} a bluebird module with state sequelize instance
   */
  getMonitorState: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.MonitorState.findOne(Utils.extend({ where: restriction || {} }, options)).then(function(state) {
        if(state === null) return reject(new Error("Could not get state.", []));
        return resolve(state);
      }).catch(function(err) {
        return reject(new Error("Could not get state.", err.errors || []));
      });
    });
  },

  /**
   * Removes a monitor state from the database.
   *
   * @param {Object} restriction - A javascript object with query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} a bluebird promise
   */
  removeMonitorState: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      return models.db.MonitorState.destroy(Utils.extend({ where: restriction }, options)).then(function() {
        return resolve();
      }).catch(function(err) {
        return reject(new Error("Could not remove state " + err.toString()));
      });
    });
  },

  /**
   * It saves ServiceInstance in database and storage it in memory
   * @param {Object} serviceObject - An object containing project values to be saved.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module. The callback is either a {ServiceInstance} data values or error
   */
  addServiceInstance: function(serviceObject, options) {
    var self = this;
    return new Promise(function(resolve, reject){
      models.db.ServiceInstance.create(serviceObject, options)
        .then(function(serviceResult){
          var service = new DataModel.Service(serviceResult);
          var logObject = serviceObject.log;
          logObject.service_instance_id = serviceResult.id;
          return models.db.Log.create(logObject, options).then(function(logResult) {
            var log = new DataModel.Log(logResult);
            service.log = log.toObject();
            return service;
          }).catch(function(err) {
            logger.error(err);
            return Utils.rollbackPromises([serviceResult.destroy()], new Error("Could not save log: " + err.message), reject);
          });
        })
        // On Success
        .then(function(serviceCreated) {
          if (serviceCreated.service_type_id === Enums.ServiceType.ALERT) {
            // save Email uri
            var serviceMetadata = Utils.generateArrayFromObject(serviceObject.metadata, function wrap(key, elm) {
              return {service_instance_id: serviceCreated.id, key: key, value: elm};
            });


            return self.addServiceMetadata(serviceMetadata, options)
              .then(function(metadata) {
                serviceCreated.setMetadata(metadata);
                return serviceCreated;
              });
          }
          return serviceCreated;
        })
        // On Success, resolve promise chain
        .then(function(serviceResource) {
          return resolve(serviceResource);
        })
        .catch(function(e) {
          logger.error(e);
          var message = "Could not save service instance: ";
          if (e.errors) { message += e.errors[0].message; }
          return reject(new Error(message));
        });
    });
  },

  addServiceMetadata: function addServiceMetadata(metadataObj, options) {
    return new Promise(function(resolve, reject) {
      models.db.ServiceMetadata.bulkCreate(metadataObj, options)
        .then(function(metadataRes) {
          return resolve(Utils.formatMetadataFromDB(metadataRes));
        })
        .catch(function(err) {
          return reject(new Error("Could not save Service Metadata due " + err.toString()));
        });
    });
  },

  upsertServiceMetadata: function upsertServiceMetadata(restriction, metadataObj, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.ServiceMetadata.findOne(Utils.extend({where: restriction}, options))
        .then(function(metaResult) {
          if (!metaResult) {
            // create new one
            return self.addServiceMetadata([metadataObj], options);
          }
          return models.db.ServiceMetadata.update(metadataObj, Utils.extend({
            fields: ["key", "value"],
            where: {
              id: metaResult.id
            }
          }, options));
        })
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(new Error("Could not update service metadata due " + err.toString()));
        });
    });
  },

  /**
   * Retrieves a list of ServiceInstances from restriction
   * @param {Object?} restriction - A javascript object with restriction query values.
   * @param {Object?} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<ServiceInstance>} A promise with Array of Service Instances
   */
  listServiceInstances: function(restriction, options) {
    return new Promise(function(resolve, reject){
      return models.db.ServiceInstance.findAll(Utils.extend({
        where: restriction,
        include: [
          {
            model: models.db.Log
          },
          {
            model: models.db.ServiceMetadata,
            required: false
          }
        ]
      }, options)).then(function(services) {
        var output = [];
        services.forEach(function(service){
          var params = Utils.extend({metadata: service.ServiceMetadata}, service.get());
          var serviceObject = new DataModel.Service(params);
          serviceObject.log = new DataModel.Log(service.Log || {});
          output.push(serviceObject);
        });

        return resolve(output);
      }).catch(function(err) {
        logger.error(err);
        return reject(new Error("Could not retrieve services " + err.message));
      });
    });
  },

  /**
   * It retrieves a service instance from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<ServiceInstance>}
   */
  getServiceInstance : function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject){
      self.listServiceInstances(restriction, Utils.extend({limit: 1}, options)).then(function(result) {
        if (result.length === 0) {
          return reject(new Error("No service instances found"));
        }

        return resolve(result[0]);
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It performs a remove operation of service from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  removeServiceInstance: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getServiceInstance(restriction, options).then(function(serviceResult) {
        // update collectors removing ID and setting them to inactive
        return self.updateCollectors({service_instance_id: serviceResult.id}, {active: false}, options)
          .then(function() {
            return models.db.ServiceInstance.destroy(Utils.extend({where: restriction}, options))
              .then(function() {
                return resolve();
              }).catch(function(err) {
                logger.error(err);
                return reject(new Error("Could not remove service instance. " + err.message));
              });
          }).catch(function(err) {
            return reject(err);
          });
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It performs a update service instance from given restriction
   *
   * @param {number} serviceId - A service identifier
   * @param {Object} serviceObject - A service object to update
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  updateServiceInstance: function(serviceId, serviceObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.getServiceInstance({id: serviceId}, options).then(function(serviceResult) {
        return models.db.ServiceInstance.update(serviceObject, Utils.extend({
            fields: ['name', 'description', 'port',
                     'numberOfThreads', 'runEnviroment', 'host',
                     'sshUser', 'sshPort', 'pathToBinary'],
            where: { id: serviceId }
          }, options))
          .then(function() {
            if (!Utils.isEmpty(serviceObject.metadata)) {
              // prepare to update/insert
              var promises = Utils.generateArrayFromObject(serviceObject.metadata, function(k, v, service) {
                var obj = {key: k, value: v, service_instance_id: service};
                return self.upsertServiceMetadata({service_instance_id: serviceId, key: k}, obj, options);
              }, serviceId);

              return Promise.all(promises);
            }
            return; // resolving chain
          })
          .then(function() {
            return resolve();
          }).catch(function(err) {
            return reject(new Error("Could not update service due " + err.toString()));
          });
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It performs a update service log from given restriction
   *
   * @param {number} logId - A log identifier
   * @param {Object} logObject - A log object values to update
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  updateLog: function(logId, logObject, options) {
    return new Promise(function(resolve, reject) {
      models.db.Log.update(logObject, Utils.extend({
        fields: ['host', 'port', 'user', 'database', 'password'],
        where: {
          id: logId
        }
      }, options)).then(function() {
        return resolve();
      }).catch(function(err) {
        return reject(new Error("Could not update log " + err.toString()));
      });
    });
  },

  /**
   * It saves DataProviderType in database.
   *
   * @param {Object} dataProviderTypeObject - An object containing needed values to create DataProviderType object.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<Object>} - a 'bluebird' module with semantics instance or error callback.
   */
  addDataProviderType: function(dataProviderTypeObject, options) {
    return new Promise(function(resolve, reject) {
      models.db.DataProviderType.create(dataProviderTypeObject, options).then(function(result) {
        return resolve(Utils.clone(result.get()));
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It retrieves a DataProviderType list object from database.
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataProviderType>} - a 'bluebird' module with DataProviderType instance or error callback
   */
  listDataProviderType: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      return models.db.DataProviderType.findAll(Utils.extend({where: restriction}, options)).then(function(result) {
        var output = [];
        result.forEach(function(element) {
          output.push(Utils.clone(element.get()));
        });

        return resolve(output);
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It retrieves a DataProviderType object from database.
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataProviderType>} - a 'bluebird' module with DataProviderType instance or error callback
   */
  getDataProviderType: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.DataProviderType.findOne(Utils.extend({where: restriction}, options)).then(function(typeResult) {
        return resolve(typeResult.get());
      }).catch(function(err) {
        logger.error(err);
        return reject(new Error("Could not retrieve DataProviderType " + err.message));
      });
    });
  },

  /**
   * It retrieves a DataProviderIntent object from database.
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataProviderIntent>} - a 'bluebird' module with DataProviderIntent instance or error callback
   */
  getDataProviderIntent: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.DataProviderIntent.findOne(Utils.extend({where: restriction}, options)).then(function(intentResult) {
        return resolve(intentResult.get());
      }).catch(function(err) {
        logger.error(err);
        return reject(new Error("Could not retrieve DataProviderIntent " + err.message));
      });
    });
  },

  /**
   * It saves DataFormat in database.
   *
   * @param {Object} dataFormatObject - An object containing needed values to create DataFormatObject object.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with semantics instance or error callback.
   */
  addDataFormat: function(dataFormatObject, options) {
    return new Promise(function(resolve, reject) {
      models.db.DataFormat.create(dataFormatObject, options).then(function(result) {
        return resolve(Utils.clone(result.get()));
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It retrieves a DataFormats list object from database.
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataFormat>} - a 'bluebird' module with DataFormat instance or error callback
   */
  listDataFormats: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.DataFormat.findAll(Utils.extend({where: restriction}, options)).then(function(dataFormats) {
        var output = [];

        dataFormats.forEach(function(dataFormat){
          output.push(Utils.clone(dataFormat.get()));
        });

        return resolve(output);
      }).catch(function(err) {
        return reject(new exceptions.DataFormatError("Could not retrieve data format", err));
      });
    });
  },

  /**
   * It saves DataSeriesSemantics in database.
   *
   * @param {Object} semanticsObject - An object containing the necessary values to create DataSeriesSemantics object.
   * @param {string} semanticsObject.name - Semantics Name
   * @param {string} semanticsObject.code - Semantics Code identifier. It must be unique
   * @param {string} semanticsObject.data_format_name - TerraMA² Data format
   * @param {string[]} dataProviderTypes - Defines a list of Data Provider types in order to determine which data provider type the semantics belongs
   * @param {Object?} semanticsMetadata - Defines a list of extra metadata of semantics
   * @param {Object?} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<Object>} - a 'bluebird' module with semantics instance or error callback.
   */
  addDataSeriesSemantics: function(semanticsObject, dataProviderTypes, semanticsMetadata, options) {
    var self = this;
    return new Promise(function(resolve, reject){
      if (!dataProviderTypes || Utils.isEmpty(dataProviderTypes)) {
        return reject(new exceptions.DataSeriesSemanticsError("Data provider types is required for new data series semantics"));
      }

      return self.listDataProviderType({name: {$in: dataProviderTypes}})
        .then(function(typesResult) {
          // ensure that given data provider types matches with retrieved
          if (typesResult.length !== dataProviderTypes.length) {
            logger.debug(semanticsObject);
            return reject(new exceptions.DataSeriesSemanticsError(Utils.format(
                "Data Provider Types does not match. Given %s but got %s",
                dataProviderTypes,
                typesResult.map(function(item) { return item.name; })
              )));
          }
          return models.db.DataSeriesSemantics.create(semanticsObject, options)
            .then(function(semantics){
              var semanticsProvidersArray = [];
              // building semantics with provider type
              for(var i = 0; i < typesResult.length; ++i) {
                var providerType = typesResult[i];
                semanticsProvidersArray.push({
                  data_provider_type_id: providerType.id,
                  data_series_semantics_id: semantics.id
                });
              }
              // add semantics data provider types
              return Promise.all([
                  semantics,
                  models.db.SemanticsProvidersType.bulkCreate(semanticsProvidersArray)
                ])
                .spread(function(dataSeriesSemantics, providersTypeBulk) {
                  if (Utils.isObject(semanticsMetadata)) {
                    var semanticsMetadataArr = [];
                    for(var k in semanticsMetadata) {
                      if (semanticsMetadata.hasOwnProperty(k)) {
                        semanticsMetadataArr.push({
                          key: k,
                          value: semanticsMetadata[k],
                          data_series_semantics_id: dataSeriesSemantics.id
                        });
                      }
                    }
                    return models.db.SemanticsMetadata.bulkCreate(semanticsMetadataArr)
                      .then(function() {
                        // returning promise chain with semantics
                        return dataSeriesSemantics;
                      });
                  }
                  // returning promise chain with semantics
                  return dataSeriesSemantics;
                });
            });
        })
        // on save process successfully
        .then(function(semantics) {
          return resolve(Utils.clone(semantics.get()));
        })
        // on any error
        .catch(function(e) {
          return reject(e);
        });
    });
  },

  /**
   * It retrieves a DataSeriesSemantics object from restriction. It should be an object containing either id identifier or
   * name identifier. This operation must retrieve only a row.
   *
   * @param {Object} restriction - An object containing DataSeriesSemantics identifier to get it.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataSeriesSemantics>} - a 'bluebird' module with DataSeriesSemantics instance or error callback
   */
  getDataSeriesSemantics: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.listDataSeriesSemantics(restriction, options).then(function(semanticsList) {
        if (semanticsList.length === 1) {
          return resolve(semanticsList[0]);
        }

        // error getting more than one or none
        return reject(new exceptions.DataSeriesSemanticsError("DataSeriesSemantics not found"));
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It retrieves a DataSeriesSemantics list object from restriction.
   *
   * @param {Object} restriction - An optional object containing DataSeriesSemantics identifier to filter it.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataSeriesSemantics>} - a 'bluebird' module with DataSeriesSemantics instance or error callback
   */
  listDataSeriesSemantics: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      return models.db.DataSeriesSemantics.findAll(Utils.extend({
        include: [{
          model: models.db.SemanticsProvidersType
        }],
        where: restriction
      }, options))
        .then(function(semanticsList) {
          var output = [];

          semanticsList.forEach(function(semantics) {
            var _semantics = semantics.get();
            _semantics.data_providers_semantics = [];

            semantics.SemanticsProvidersTypes.forEach(function(semanticsProvider) {
              _semantics.data_providers_semantics.push(semanticsProvider.get());
            });

            output.push(_semantics);
          });

          return resolve(output);
        }).catch(function(err) {
          return reject(new exceptions.DataSeriesSemanticsError("Could not retrieve data series semantics " + err.toString()));
        });
    });
  },

  /**
   * It retrieves binding between Semantics and Data Provider types
   *
   * @param {Object} restriction - a query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<Array<Object>>}
   */
  listSemanticsProvidersType: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.SemanticsProvidersType.findAll(Utils.extend({where: restriction}, options))
        .then(function(semanticsProvidersResult) {
          var output = [];
          semanticsProvidersResult.forEach(function(element) {
            output.push(element.get());
          });
          return resolve(output);
        }).catch(function(err) {
          logger.error(err);
          return reject(err);
        });
    });
  },
/**
   * It performs Insert or Update operation in database. If data set format found with given restriction, it applies
   * update operation. Otherwise, it performs insert operation
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} dataProviderOptions - A data provider configuration values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module
   */
  upsertDataProviderOptions: function(restriction, dataProviderOption, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return self
        .listDataProviderOptions(restriction, Utils.extend({limit: 1}, options))
        .then(function(dataProviderOptions) {
          if (dataProviderOptions.length === 0) {
            // insert
            return models.db.DataProviderOptions.create(dataProviderOption, options);
          } else {
            return models.db.DataProviderOptions.update(dataProviderOption, Utils.extend({
              fields: ["key", "value", "data_provider_id"],
              where: {id: dataProviderOptions[0].id}
            }, options));
          }
        })

        .then(function() {
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error(Utils.format("Could not upsert data provider configuration %s", err.toString())));
        });
    });
  },
  /**
   * It retrieves all data provider configurations from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  listDataProviderOptions: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.DataProviderOptions
        .findAll(Utils.extend({
          where: restriction
        }, options))
        .then(function(dataProviderOptions) {
          return resolve(dataProviderOptions);
        })

        .catch(function(err) {
          return reject(new Error(Utils.format("Could not retrieve data provider configuration %s", err.toString())));
        });
    });
  },
  /**
   * It saves DataProvider in database and load it in memory
   *
   * @param {Object} dataProviderObject - An object containing needed values to create DataProvider object.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataProvider>} - a 'bluebird' module with DataProvider instance or error callback
   */
  addDataProvider: function(dataProviderObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataProvider.create(dataProviderObject, options).then(function(dataProvider){
        if (dataProviderObject.configuration){
          var configurationList = [];
          var configuration = dataProviderObject.configuration;

          for(var key in configuration) {
            if (configuration.hasOwnProperty(key)) {
              configurationList.push({
                data_provider_id: dataProvider.id,
                key: key,
                value: configuration[key]
              });
            }
          }
          return models.db.DataProviderOptions.bulkCreate(configurationList, Utils.extend({data_provider_id: dataProvider.id}, options)).then(function () {
            return models.db.DataProviderOptions.findAll(Utils.extend({where: {data_provider_id: dataProvider.id}}, options)).then(function(dataSetFormats) {
              return dataProvider.getDataProviderType().then(function(dataProviderType) {
                var dataProviderObject = dataProvider.get();
                dataSetFormats.forEach(function(dSetFormat){
                  dataProviderObject[dSetFormat.key] = dSetFormat.value;
                });
                var dProvider = new DataModel.DataProvider(dataProvider.get());
                dProvider.data_provider_type = dataProviderType.get();
                self.data.dataProviders.push(dProvider);

                return resolve(dProvider);
              }).catch(function(err) {
                logger.error(err);
                return reject(err);
              });
            }).catch(function(err){
              logger.error(err);
              return reject(err);
            });
          }).catch(function(err){
            logger.error(err);
            return reject(err);
          });
        } else {
          return dataProvider.getDataProviderType().then(function(dataProviderType) {
            var dProvider = new DataModel.DataProvider(dataProvider.get());
            dProvider.data_provider_type = dataProviderType.get();
            self.data.dataProviders.push(dProvider);

            return resolve(dProvider);
          }).catch(function(err) {
            logger.error(err);
            return reject(err);
          });
        }
      }).catch(function(err){
        var message = "Could not save data provider due: ";
        logger.error(err.errors);
        if (err.errors) {
          err.errors.forEach(function(e) { message += e.message + "; "; });
        } else {
          message += err.message;
        }
        return reject(new exceptions.DataProviderError(message, err.errors));
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
      var dataProvider = Utils.find(self.data.dataProviders, restriction);
      if (dataProvider) {
        return resolve(new DataModel.DataProvider(dataProvider));
      } else {
        return reject(new exceptions.DataProviderError(
          "Could not find a data provider: " + restriction[Object.keys(restriction)[0]]));
      }
    });
  },

  /**
   * It retrieves DataProviders loaded in memory.
   *
   * @param {Object} restriction - An object containing DataProvider filter values
   * @returns {Array<DataProvider>} - An array with DataProviders available/loaded in memory.
   */
  listDataProviders: function(restriction) {
    var dataProviderObjectList = [];

    if (restriction === undefined || restriction === null) {
      restriction = {};
    }

    Utils.filter(this.data.dataProviders, restriction).forEach(function(dataProvider) {
      dataProviderObjectList.push(new DataModel.DataProvider(dataProvider));
    });
    return dataProviderObjectList;
  },

  /**
   * It updates a DataProvider instance from object.
   *
   * @param {int} dataProviderId - A DataProvider identifier to get it.
   * @param {Object} dataProviderObject - An object containing DataProvider to update.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with DataProvider instance or error callback
   */
  updateDataProvider: function(dataProviderId, dataProviderObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataProvider = Utils.find(self.data.dataProviders, {id: dataProviderId});

      if (dataProvider) {
        models.db.DataProvider.update(dataProviderObject, Utils.extend({
          fields: ["name", "description", "uri", "active"],
          where: {
            id: dataProvider.id
          }
        }, options)).then(function() {
          if (dataProviderObject.name) { dataProvider.name = dataProviderObject.name; }

          if (dataProviderObject.timeout) { dataProvider.timeout = dataProviderObject.timeout; }

          if (dataProviderObject.description) { dataProvider.description = dataProviderObject.description; }

          if (dataProviderObject.uri) { dataProvider.uri = dataProviderObject.uri; }

          dataProvider.active = dataProviderObject.active;

          if (dataProviderObject.configuration){
            var dataProviderConfPromises = [];

            for (var key in dataProviderObject.configuration){
              if (dataProviderObject.configuration.hasOwnProperty(key)){
                var configRestriction = {
                  data_provider_id: dataProviderId,
                  key: key
                };
                var configObject = {
                  value: dataProviderObject.configuration[key]
                }
              }
              dataProviderConfPromises.push(self.upsertDataProviderOptions(configRestriction, configObject, options));
            }
            return Promise.all(dataProviderConfPromises).then(function(dataProvConfigs){
              dataProvider.timeout = dataProviderObject.configuration.timeout;
              dataProvider.active_mode = dataProviderObject.configuration.active_mode;
              return resolve(new DataModel.DataProvider(dataProvider));
            });
          } else {
            return resolve(new DataModel.DataProvider(dataProvider));
          }
        }).catch(function(err) {
          return reject(new exceptions.DataProviderError("Could not update data provider ", err));
        });
      } else { reject(new exceptions.DataProviderError("Data provider not found")); }
    });
  },

  /**
   * It removes DataProvider from param. It should be an object containing either id identifier or
   * name identifier.
   *
   * @param {Object} dataProviderParam - An object containing DataProvider identifier to get it.
   * @param {Boolean} cascade - A bool value to delete on cascade
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with DataProvider instance or error callback
   */
  removeDataProvider: function(dataProviderParam, cascade, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      if (!cascade) { cascade = false; }

      var provider = Utils.remove(self.data.dataProviders, dataProviderParam);
      if (provider) {
        return models.db.DataProvider.destroy(Utils.extend({where: {id: provider.id}}, options)).then(function() {
          // remove data series
          var dataSeriesList = Utils.removeAll(self.data.dataSeries, {data_provider_id: provider.id});
          dataSeriesList.forEach(function(dataSeries) {
            var dSets = Utils.removeAll(self.data.dataSets, {data_series_id: dataSeries.id});
          });

          return resolve({dataProvider: provider, dataSeries: dataSeriesList});
        }).catch(function(err) {
          logger.error(err);
          return reject(new exceptions.DataProviderError("Could not remove DataProvider with a collector associated", err));
        });
      } else {
        return reject(new exceptions.DataManagerError("DataProvider not found"));
      }
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
      var dataSerie = Utils.find(self.data.dataSeries, restriction);
      if (dataSerie) {
        return resolve(new DataModel.DataSeries(dataSerie));
      } else {
        return reject(new exceptions.DataSeriesError("Could not find a data series: " + restriction[Object.keys(restriction)]));
      }
    });
  },

  /**
   * It retrieves DataSeries loaded in memory.
   *
   * @param {Object} restriction - an object to filter result
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<Array<DataSeries>>} - An array with DataSeries available/loaded in memory.
   */
  listDataSeries: function(restriction, options) {
    var self = this;
    if (!restriction) { restriction = {}; }

    return new Promise(function(resolve, reject) {
      var dataSeriesList = [];
      if (restriction.hasOwnProperty("schema")) {
        if (restriction.schema === "all") {
          self.listDataSeries({"Collector": restriction}).then(function(data) {
            var staticRestriction = {
              data_series_semantics: { temporality: Enums.TemporalityType.STATIC }
            };

            if(restriction.dataProvider !== undefined)
              staticRestriction['dataProvider'] = restriction.dataProvider;

            return self.listDataSeries(staticRestriction, options).then(function(staticData) {
              var output = [];
              data.forEach(function(d) {
                output.push(d);
              });
              staticData.forEach(function(d) {
                output.push(d);
              });

              return resolve(output);
            }).catch(function(err) {
              return reject(err);
            });
          }).catch(function(err) {
            return reject(err);
          });
        }
      } else if (restriction && restriction.hasOwnProperty("Collector")) {
        // collector restriction
        self.listCollectors({}, options).then(function(collectorsResult) {
          var collectorFilter = new Filters.CollectorFilter();
          var output = collectorFilter.match(collectorsResult, {
            dataSeries: self.data.dataSeries
          });

          var copyRestriction = Utils.makeCopy(restriction, null);

          if(copyRestriction.Collector.dataProvider !== undefined)
            copyRestriction.dataProvider = copyRestriction.Collector.dataProvider;

          delete copyRestriction.Collector;
          // collect output and processing
          return resolve(Utils.filter(output, copyRestriction));
        }).catch(function(err) {
          return reject(err);
        });
      } else if (restriction && restriction.hasOwnProperty("Analysis")) {
        return self.listAnalysis({}, options)
          .then(function(analysisList) {
            var analysisFilter = new Filters.AnalysisFilter();
            return resolve(analysisFilter.match(analysisList, {dataSeries: self.data.dataSeries}));
          })

          .catch(function(err) { return reject(err); });
      } else {
        var dataSeriesFound = Utils.filter(self.data.dataSeries, restriction);
        dataSeriesFound.forEach(function(dataSeries) {
          dataSeriesList.push(new DataModel.DataSeries(dataSeries));
        });
        return resolve(dataSeriesList);
      }
    });
  },

  /**
   * It saves a DataSeries object in database. It also saves DataSet if there are.
   *
   * @param {Object} dataSeriesObject - An object containing DataSeries values to save it.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  addDataSeries: function(dataSeriesObject, analysisType, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var output;
      models.db.DataSeries.create(dataSeriesObject, options).then(function(dataSerie){
        var obj = dataSerie.get();

        // getting semantics
        dataSerie.getDataSeriesSemantic().then(function(dataSemantics) {
          obj.DataSeriesSemantic = dataSemantics;
          output = new DataModel.DataSeries(obj);

          // if there DataSets to save too
          if (dataSeriesObject.dataSets && dataSeriesObject.dataSets.length > 0) {
            var dataSets = [];
            for(var i = 0; i < dataSeriesObject.dataSets.length; ++i) {
              var dSet = dataSeriesObject.dataSets[i];
              dSet.data_series_id = dataSerie.id;
              dataSets.push(self.addDataSet(dataSemantics.get(), dSet, analysisType, options));
            }

            return Promise.all(dataSets).then(function(dataSets){
              var dataSeriesInstance = new DataModel.DataSeries(output);
              dataSeriesInstance.dataSets = dataSets;

              var semanticsList = Application.get("semantics");
              var semantic = semanticsList.find(function(dSeriesSemantic){
                return dataSeriesInstance.data_series_semantics.code == dSeriesSemantic.code;
              });
              Object.assign(dataSeriesInstance.data_series_semantics, semantic);
              dataSeriesInstance.semantics = dataSeriesInstance.data_series_semantics;
              self.data.dataSeries.push(dataSeriesInstance);

              // get DataProvider object
              return self.getDataProvider({id: dataSerie.data_provider_id}).then(function(dProvider) {
                dataSeriesInstance.setDataProvider(dProvider);

                // resolving promise
                return resolve(dataSeriesInstance);
              });
            }).catch(function(err) {
              return reject(err);
            });
          } else {
            // rollback data series
            return reject(new exceptions.DataSeriesError("Could not save DataSeries without any data set."));
          }
        }).catch(function(err) {
          return reject(err);
        });
      }).catch(function(err){
        logger.error(err);
        return reject(new exceptions.DataSeriesError("Could not save data series due: ", err.errors || []));
      });
    });
  },
  /**
   * It retrieves all data set formats from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  listDataSetFormats: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.DataSetFormat
        .findAll(Utils.extend({
          where: restriction
        }, options))
        .then(function(dataSetFormats) {
          return resolve(dataSetFormats);
        })

        .catch(function(err) {
          return reject(new Error(Utils.format("Could not retrieve data set formats %s", err.toString())));
        });
    });
  },
  /**
   * It performs Insert or Update operation in database. If data set format found with given restriction, it applies
   * update operation. Otherwise, it performs insert operation
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} dataSetFormat - A data set format values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  upsertDataSetFormats: function(restriction, dataSetFormat, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return self
        .listDataSetFormats(restriction, Utils.extend({limit: 1}, options))
        .then(function(dataSetFormats) {
          if (dataSetFormats.length === 0) {
            // insert
            return models.db.DataSetFormat.create(dataSetFormat, options);
          } else {
            return models.db.DataSetFormat.update(dataSetFormat, Utils.extend({
              fields: ["key", "value", "data_set_id"],
              where: {id: dataSetFormats[0].id}
            }, options));
          }
        })

        .then(function() {
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error(Utils.format("Could not upsert data set format %s", err.toString())));
        });
    });
  },
  /**
   * It updates a DataSeries object. It should be an object containing object filled out with identifier
   * and model values.
   *
   * @param {Number} dataSeriesId - A DataSeries id
   * @param {Object} dataSeriesObject - An object containing DataSeries identifier to get it.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  updateDataSeries: function(dataSeriesId, dataSeriesObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataSeries = Utils.find(self.data.dataSeries, {id: dataSeriesId});

      if (!dataSeries) {
        return reject(new exceptions.DataSeriesError("Data series not found. ", []));
      }

      return models.db.DataSeries.update(dataSeriesObject, Utils.extend({
        fields: ['name', 'description', 'data_provider_id', 'active', 'data_series_semantics_id'],
        where: {
          id: dataSeriesId
        }
      }, options))
      // on data series update, update format
        .then(function() {
          return self.getDataSeriesSemantics({id: dataSeriesObject.data_series_semantics_id}, options);
        })

        .then(function(dataSeriesSemantics) {
          var semanticsList = Application.get("semantics");
          var semantic = semanticsList.find(function(dSeriesSemantic){
            return dataSeriesSemantics.code == dSeriesSemantic.code;
          });
          Object.assign(dataSeriesSemantics, semantic);
          dataSeries.semantics = dataSeriesSemantics;
          dataSeries.data_series_semantics = dataSeriesSemantics;
          /**
           * Helper to iterate over formats in order to build promise "upsertDataSetFormats"
           *
           * @param {string} key - Format Key
           * @param {string} value - Format value
           * @param {DataSet} extra - Current data set.
           * @returns {Promise}
           */
          function formatIterator(key, value, extra) {
            var obj = {
              key: key,
              value: value,
              data_set_id: extra.id
            };
            // performs Insert or Update
            return self.upsertDataSetFormats({
              data_set_id: extra.id,
              key: key
            }, obj, options)
              .then(function() {
                return obj;
              });
          }

          /**
           * It defines a promises of update data set formats.
           * @type {Promise}
           */
          var promises = [];

          if(dataSeriesObject.removedDcps !== undefined) {
            dataSeriesObject.removedDcps.forEach(function(dataSetIdToRemove) {
              dataSeries.dataSets.forEach(function(completeDataSet) {
                if(completeDataSet.format._id == dataSetIdToRemove) {
                  var removeProvider = self.removeDataSet(completeDataSet).then(function(returned) {
                    var index = dataSeries.dataSets.indexOf(returned);
                    if(index > -1)
                      dataSeries.dataSets.splice(index, 1);
                  });
                  promises.push(removeProvider);
                }
              });
            });
          } else {
            // check if must remove a data set
            var dataSetsToRemove = [];
            if (dataSeriesObject.dataSets.length < dataSeries.dataSets.length){
              dataSeries.dataSets.forEach(function(dataSetToRemove){
                var dontRemove = dataSeriesObject.dataSets.some(function(dataSetToCompare){
                  return dataSetToCompare.format._id == dataSetToRemove.format._id;
                });
                if (!dontRemove){
                  dataSetsToRemove.push(dataSetToRemove);
                }
              });
            }

            //Removing datasets
            dataSetsToRemove.forEach(function(dataSetId){
              var removeProvider = self.removeDataSet(dataSetId).then(function(returned){
                var index = dataSeries.dataSets.indexOf(returned);
                if (index > -1){
                  dataSeries.dataSets.splice(index, 1);
                }
              });
              promises.push(removeProvider);
            });
          }

          (dataSeriesObject.editedDcps !== undefined ? dataSeriesObject.editedDcps : dataSeriesObject.dataSets).forEach(function(newDataSet) {
            var dataSetToUpdate = dataSeries.dataSets.find(function(dSet){
              return dSet.format._id == newDataSet.format._id;
            });
            // Update data set
            if(dataSetToUpdate) {
              dataSetToUpdate.semantics = dataSeriesSemantics;
              var updatePromise = self.updateDataSet(dataSetToUpdate, newDataSet, options)
              .then(function(dataSetUpdated) {
                var promisesFormatArray = [];
                for(var j = 0; j < dataSeries.dataSets.length; ++j) {
                  var dataSet = dataSeries.dataSets[j];

                  if(dataSetUpdated.id === dataSet.id) {
                    var promisesFormat = Utils.generateArrayFromObject(dataSetUpdated.format, formatIterator, dataSet);
                    promisesFormat.forEach(function(promiseFormat){
                      promisesFormatArray.push(promiseFormat);
                    });
                    dataSet.active = dataSetUpdated.active;
                    dataSet.format = dataSetUpdated.format;
                    if(dataSetUpdated.position) {
                      dataSet.position = dataSetUpdated.position;
                      promisesFormatArray.push(self.getWKT(dataSetUpdated.position).then(function(posWkt){
                        dataSet.positionWkt = posWkt;
                      }));
                    }
                  }
                }

                return Promise.all(promisesFormatArray);
              });
              promises.push(updatePromise);
              // add data set
            } else {
              newDataSet.data_series_id = dataSeriesId;
              var addPromise = self.addDataSet(dataSeriesSemantics, newDataSet).then(function(newDSet){
                dataSeries.dataSets.push(newDSet);
              });
              promises.push(addPromise);
            }
          });

          return Promise.all(promises);
        })
        // on successfully updating data sets
        .then(function() {
          return self.getDataProvider({id: parseInt(dataSeriesObject.data_provider_id)});
        })
        // retrieving data provider
        .then(function(dataProvider) {
          dataSeries.name = dataSeriesObject.name;
          dataSeries.description = dataSeriesObject.description;
          dataSeries.data_provider_id = dataProvider.id;
          dataSeries.active = dataSeriesObject.active;
          dataSeries.data_series_semantics_id = dataSeriesObject.data_series_semantics_id;

          return resolve(new DataModel.DataSeries(dataSeries));
        }).catch(function(err) {
          return reject(new exceptions.DataSeriesError(Utils.format("Could not update data series %s", err.toString()), err.errors));
        });
    });
  },

  /**
   * It removes a DataSeries object. It should be an object containing object filled out with identifier
   * and model values.
   *
   * @param {Object} dataSeriesParam - An object containing DataSeries identifier to remove it.
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  removeDataSerie: function(dataSeriesParam, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataSeries.destroy(Utils.extend({where: dataSeriesParam}, options))
        .then(function (status) {
          // Removing data set from memory. Its not necessary to remove in database, since on remove cascade is enabled.
          Utils.remove(self.data.dataSeries, dataSeriesParam);
          Utils.removeAll(self.data.dataSets, {data_series_id: dataSeriesParam.id});
          return resolve(status);
        }).catch(function (err) {
          logger.error(err);
          return reject(new exceptions.DataSeriesError("Could not remove DataSeries " + err.message));
        });
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
   *   "position" : GeoJSON object syntax
   * }
   *
   * @param {string} dataSeriesSemantic - A string value representing DataSet type. (dcp, occurrence, grid).
   * @param {Array<Object>} dataSetObject - An object containing DataSet values to save it.
   * @param {string} analysisType -
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  addDataSet: function(dataSeriesSemantic, dataSetObject, analysisType, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.DataSet.create({
        active: dataSetObject.active,
        data_series_id: dataSetObject.data_series_id
      }, options).then(function(dataSet) {

        var onSuccess = function(dSet) {
          var output;
          output = DataModel.DataSetFactory.build(Object.assign(Utils.clone(dSet.get()), dataSet.get()));
          logger.debug(output);
          output.semantics = dataSeriesSemantic;

          // save dataformat
          if (dataSetObject.format) {
            var formats = dataSetObject.format;
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
            }

            return models.db.DataSetFormat.bulkCreate(formatList, Utils.extend({data_set_id: dataSet.id}, options)).then(function () {
              return models.db.DataSetFormat.findAll(Utils.extend({where: {data_set_id: dataSet.id}}, options)).then(function(dataSetFormats) {
                output.format = {};
                dataSetFormats.forEach(function(dataSetFormat) {
                  output.format[dataSetFormat.key] = dataSetFormat.value;
                });

                // TODO: check it. if analysis, add input dataseries id
                if (analysisType && analysisType.data_series_id) {
                  output.format.monitored_object_dataseries_id = analysisType.data_series_id;
                }
                self.data.dataSets.push(output);
                return resolve(output);
              });
            }).catch(function (err) {
              logger.error(err);
              return reject(new exceptions.DataFormatError("Could not save data format: ", err));
            });
          } else {// todo: validate it
            self.data.dataSets.push(output);
            return resolve(output);
          }
        };

        var onError = function(err) {
          logger.error(err);
          return reject(new exceptions.DataSetError("Could not save data set." + err.message));
        };

        // rollback data set function if any error occurred
        var rollback = function(dataSet) {
          return dataSet.destroy().then(function() {
            logger.error("rollback");
            return reject(new exceptions.DataSetError("Invalid data set type. DataSet destroyed"));
          }).catch(onError);
        };

        if (dataSeriesSemantic && dataSeriesSemantic instanceof Object) {
          switch(dataSeriesSemantic.data_series_type_name) {
            case DataSeriesType.DCP:
              var dataSetDcp = {
                data_set_id: dataSet.id,
                position: dataSetObject.position
              };
              models.db.DataSetDcp.create(dataSetDcp, options).then(function(dSetDcp) {
                // TODO: reuse it. It retrieving entire GeoJSON representation (with SRID)
                models.db.DataSetDcp.findOne(Utils.extend({
                  attributes: [
                    "id",
                    "data_set_id",
                    [orm.fn('ST_AsGeoJSON', orm.col('position'), 15, 2), 'position'],
                    [orm.fn('ST_AsEwkt', orm.col('position')), 'positionWkt']
                  ],
                  where: {
                    id: dSetDcp.id
                  }
                }, options)).then(onSuccess).catch(onError);
              }).catch(onError);
              break;
            case DataSeriesType.OCCURRENCE:
              models.db.DataSetOccurrence.create({data_set_id: dataSet.id}, options).then(onSuccess).catch(onError);
              break;
            case DataSeriesType.GEOMETRIC_OBJECT:
              onSuccess(dataSet);
              break;
            case DataSeriesType.GRID:
              models.db.DataSetGrid.create({data_set_id: dataSet.id}, options).then(onSuccess).catch(onError);
              break;
            case DataSeriesType.ANALYSIS_MONITORED_OBJECT:
              models.db.DataSetMonitored.create({data_set_id: dataSet.id}, options).then(onSuccess).catch(onError);
              break;
            default:
              if (!options && !options.transaction) {
                rollback(dataSet);
              }
          }
        } else {
          rollback(dataSet);
        }
      }).catch(function(err) {
        return reject(err);
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
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with DataSeries instance or error callback
   */
  getDataSet: function(restriction, format, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      // setting default format
      if (format === undefined) { format = Enums.Format.GEOJSON; }

      if (format !== Enums.Format.GEOJSON && format !== Enums.Format.WKT) {
        return reject(new exceptions.DataSetError("Invalid output format while retrieving dataset"));
      }
      var dataSet = Utils.find(self.data.dataSets, restriction);
      if (dataSet) {
        var output = DataModel.DataSetFactory.build(dataSet);

        if (output.position && format === Enums.Format.WKT) {
          // Getting wkt representation of Point from GeoJSON
          return self.getWKT(output.position).then(function (wktGeom) {
            output.positionWkt = wktGeom;
            return resolve(output);
          }).catch(function (err) {
            return reject(err);
          });
        } else {
          return resolve(output);
        }
      } else {
        return reject(new exceptions.DataSetError("Could not find a data set: ", restriction));
      }
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
  updateDataSet: function(restriction, dataSetObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {

      var dataSet = Utils.find(self.data.dataSets, {id: restriction.id});

      if (dataSet) {
        return models.db.DataSet.findById(dataSet.id, options)
          .then(function(result) {
            return result.updateAttributes({active: dataSetObject.active}, options)
              .then(function() {
                return result.getDataSet(restriction.semantics.data_series_type_name, options)
                  .then(function(dSet) {
                    return dSet.updateAttributes(dataSetObject, options)
                      .then(function() {
                        return result.getDataSetFormats(options)
                          .then(function(dSetFormat){
                            var output = Utils.clone(result.get());
                            output.class = "DataSet";
                            switch (restriction.semantics.data_series_type_name) {
                              case DataSeriesType.DCP:
                                output.position = Utils.clone(dSet.position);
                                break;
                            }
                            /**
                             * Stringification process. It is important to force cast to string due the formats here
                             * may be cast to int/float etc.
                             */
                            var formatStringfied = {};
                            for (var key in dataSetObject.format) {
                              if (dataSetObject.format.hasOwnProperty(key)) {
                                formatStringfied[key] = String(dataSetObject.format[key]);
                              }
                            }
                            output.active = result.active;
                            output.format = formatStringfied;
                            return resolve(output);
                          }); // end result.getDataSetFormats
                    }); // end dSet.updateAttributes
                }); // end result.getDataSet
            }); // end result.updateAttributes
          }).catch(function(err) {
            logger.error(err);
            return reject(err);
          });
      } else {
        return reject(new exceptions.DataSeriesError("Could not find a data set: ", restriction));
      }
    });
  },

  /**
   * It updates a DataSet 'active' attribute.
   *
   * @param {integer} id - Id of the DataSet.
   * @param {boolean} active - Flag that indicates the new state of the DataSet.
   * @return {Promise} - a 'bluebird' module with DataSet instance or error callback
   */
  updateDataSetState: function(id, active) {
    var self = this;
    return new Promise(function(resolve, reject) {

      var dataSet = Utils.find(self.data.dataSets, { id: id });

      if(dataSet) {
        models.db.DataSet.findById(id).then(function(result) {
          result.updateAttributes({active: active}).then(function() {
            dataSet.active = active;

            resolve(result);
          }).catch(function(err) {
            reject(err);
          });
        }).catch(function(err) {
          reject(err);
        });
      } else {
        reject(new exceptions.DataSeriesError("Could not find a data set: ", id));
      }
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
          models.db.DataSet.destroy({where: {id: dataSet.id}}).then(function(status) {
            resolve(dataSetId);
            self.data.dataSets.splice(index, 1);
          }).catch(function(err) {
            reject(err);
          });
          return;
        }
      }

      reject(new exceptions.DataSetError("Data set not found."));
    });
  },

  /**
   * It retrieves a list of DataSets in memory.
   *
   * @return {Array<DataSet>} - a 'bluebird' module with DataSeries instance or error callback
   */
  listDataSets: function() {
    var dataSetsList = [];
    for(var index = 0; index < this.data.dataSets.length; ++index) {
      dataSetsList.push(Utils.clone(this.data.dataSets[index]));
    }

    return dataSetsList;
  },
  /**
   * It performs a collector insertion, using cascade addDataSeries, addSchedule and addCollector operations.
   *
   * @deprecated It will be deprecated soon (Beta-1). Use addDataSeries, addSchedule and addCollector instead
   * @param {Object} dataSeriesObject
   * @param {Object} scheduleObject
   * @param {Object} filterObject
   * @param {Object} serviceObject
   * @param {Object[]} intersectionArray
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<DataModel.Collector>}
   */
  addDataSeriesAndCollector: function(dataSeriesObject, scheduleObject, filterObject, serviceObject, intersectionArray, active, options) {
    /**
     * @type {DataManager}
     */
    var self = this;

    return new Promise(function(resolve, reject) {
      return self.addDataSeries(dataSeriesObject.input, null, options).then(function(dataSeriesResult) {
        return self.addDataSeries(dataSeriesObject.output, null, options).then(function(dataSeriesResultOutput) {
          return self.addSchedule(scheduleObject, options).then(function(scheduleResult) {
            var schedule;
            if (scheduleResult){
              schedule = new DataModel.Schedule(scheduleResult);
            }
            var collectorObject = {};

            // todo: get service_instance id and collector status (active)
            collectorObject.data_series_input = dataSeriesResult.id;
            collectorObject.data_series_output = dataSeriesResultOutput.id;
            collectorObject.service_instance_id = serviceObject.id;
            collectorObject.schedule_type = scheduleObject.scheduleType;
            if (scheduleObject.scheduleType == Enums.ScheduleType.SCHEDULE){
              collectorObject.schedule_id = scheduleResult.id;
            }
            collectorObject.active = active;
            collectorObject.collector_type = 1;

            return self.addCollector(collectorObject, filterObject, options).then(function(collectorResult) {
              var collector = collectorResult;

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
                return models.db.Intersection.bulkCreate(intersectionArray, Utils.extend({returning: true}, options))
                  .then(function(bulkIntersectionResult) {
                    collector.setIntersection(bulkIntersectionResult);
                    return resolve({
                      collector: collector,
                      input: dataSeriesResult,
                      output:dataSeriesResultOutput,
                      schedule: schedule,
                      intersection: bulkIntersectionResult
                    });
                  }).catch(function(err) {
                    return reject(new exceptions.AnalysisError("Could not save data series " + err.toString()));
                  });
              }
            }).catch(function(err) {
              // rollback schedule
              return reject(err);
            });
          }).catch(function(err) {
            // rollback data series
            logger.error(err);
            return reject(err);
          });
        }).catch(function(err) {
          return reject(err);
        });
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It performs a add schedule in database
   *
   * @param {Object} scheduleObject - A query values to insert
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<Schedule>}
   */
  addSchedule: function(scheduleObject, options) {
    return new Promise(function(resolve, reject) {
      if (scheduleObject.scheduleType == Enums.ScheduleType.AUTOMATIC){
        models.db.AutomaticSchedule.create(scheduleObject, options).then(function(schedule) {
          return resolve(new DataModel.AutomaticSchedule(schedule.get()));
        }).catch(function(err) {
          // todo: improve error message
          return reject(new exceptions.ScheduleError("Could not save schedule. " + err.toString()));
        });
      } else if (scheduleObject.scheduleType == Enums.ScheduleType.SCHEDULE || scheduleObject.scheduleType == Enums.ScheduleType.REPROCESSING_HISTORICAL){
        models.db.Schedule.create(scheduleObject, options).then(function(schedule) {
          return resolve(new DataModel.Schedule(schedule.get()));
        }).catch(function(err) {
          // todo: improve error message
          return reject(new exceptions.ScheduleError("Could not save schedule. " + err.toString()));
        });
      } else {
        return resolve();
      }
    });
  },

  /**
   * It performs update schedule from given restriction
   *
   * @param {number} scheduleId - A schedule identifier
   * @param {Object} scheduleObject - A query values to update
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  updateSchedule: function(scheduleId, scheduleObject, options) {
    return new Promise(function(resolve, reject) {
      models.db.Schedule.update(scheduleObject, Utils.extend({
        fields: ['schedule', 'schedule_time', 'schedule_unit', 'frequency_unit', 'frequency', 'frequency_start_time'],
        where: {
          id: scheduleId
        }
      }, options))
        .then(function() {
          return resolve();
        }).catch(function(err) {
          return reject(new exceptions.ScheduleError("Could not update schedule " + err.toString()));
        });
    });
  },

  /**
   * It performs update automatic schedule from given restriction
   *
   * @param {number} scheduleId - A schedule identifier
   * @param {Object} scheduleObject - A query values to update
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  updateAutomaticSchedule: function(scheduleId, scheduleObject, options) {
    return new Promise(function(resolve, reject) {
      models.db.AutomaticSchedule.update(scheduleObject, Utils.extend({
        fields: ['data_ids'],
        where: {
          id: scheduleId
        }
      }, options))
        .then(function() {
          return resolve();
        }).catch(function(err) {
          return reject(new exceptions.ScheduleError("Could not update schedule " + err.toString()));
        });
    });
  },

  /**
   * It performs delete schedule from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  removeSchedule: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.Schedule.destroy(Utils.extend({where: {id: restriction.id}}, options)).then(function() {
        return resolve();
      }).catch(function(err) {
        logger.error(err);
        return reject(new exceptions.ScheduleError("Could not remove schedule " + err.message));
      });
    });
  },

  /**
   * It performs delete automatic schedule from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  removeAutomaticSchedule: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.AutomaticSchedule.destroy(Utils.extend({where: {id: restriction.id}}, options)).then(function() {
        return resolve();
      }).catch(function(err) {
        logger.error(err);
        return reject(new exceptions.ScheduleError("Could not remove schedule " + err.message));
      });
    });
  },

  /**
   * It retrieves a schedule from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Schedule>}
   */
  getSchedule: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.Schedule.findOne(Utils.extend({
        where: restriction || {}
      }, options)).then(function(schedule) {
        if (schedule) {
          return resolve(new DataModel.Schedule(schedule.get()));
        }
        return reject(new exceptions.ScheduleError("Could not find schedule"));
      });
    });
  },
  /**
   * It retrieves a automatic schedule from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Schedule>}
   */
  getAutomaticSchedule: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.AutomaticSchedule.findOne(Utils.extend({
        where: restriction || {}
      }, options)).then(function(schedule) {
        if (schedule) {
          return resolve(new DataModel.AutomaticSchedule(schedule.get()));
        }
        return reject(new exceptions.ScheduleError("Could not find automatic schedule"));
      });
    });
  },

  /**
   * It retrieves all automatic schedule in database from given restriction
   *
   * @param {Object} restriction - A automatic schedule restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  listAutomaticSchedule: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.AutomaticSchedule.findAll(Utils.extend({
        where: restriction || {}
      }, options)).then(function(automaticSchedulesResult){
        return resolve(automaticSchedulesResult.map(function(automaticSchedule){
          return new DataModel.AutomaticSchedule(automaticSchedule.get());
        }));
      });
    });
  },

  /**
   * It performs add collector in database
   *
   * @param {Object} collectorObject - A javascript object with collector values
   * @param {Object} filterObject - A javascript object with filter values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Collector>}
   */
  addCollector: function(collectorObject, filterObject, options) {
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.Collector.create(collectorObject, options).then(function(collectorResult) {
        var collector = new DataModel.Collector(collectorResult.get());

        var schedule;
        var dataSeriesInput;
        var getSchedulePromise;

        if (collectorResult.schedule_type == Enums.ScheduleType.MANUAL){
          getSchedulePromise = Promise.resolve();
        } else {
          getSchedulePromise = self.getSchedule({id: collectorResult.schedule_id}, options);
        }
        return getSchedulePromise
          .then(function(scheduleResult) {
            schedule = scheduleResult;
            return self.getDataSeries({id: collectorResult.data_series_input});
          })

          .then(function(dataSeriesInputResult) {
            dataSeriesInput = dataSeriesInputResult;
            return self.getDataSeries({id: collectorResult.data_series_output});
          })

          .then(function(dataSeriesOutput) {
            var inputOutputArray = [];

            for(var i = 0; i < dataSeriesInput.dataSets.length; ++i) {
              var inputDataSet = dataSeriesInput.dataSets[i];
              var outputDataSet;
              if (dataSeriesOutput.dataSets.length === 1) {
                outputDataSet = dataSeriesOutput.dataSets[0];
              } else {
                outputDataSet = dataSeriesOutput.dataSets[i];
              }

              inputOutputArray.push({
                collector_id: collectorResult.id,
                input_dataset: inputDataSet.id,
                output_dataset: outputDataSet.id
              });
            }

            return models.db.CollectorInputOutput.bulkCreate(inputOutputArray, options).then(function(bulkInputOutputResult) {
              var input_output_map = [];
              bulkInputOutputResult.forEach(function(bulkResult) {
                input_output_map.push({
                  input: bulkResult.input_dataset,
                  output: bulkResult.output_dataset
                });
              });
              collector.input_output_map = input_output_map;
              collector.schedule = schedule;

              if (_.isEmpty(filterObject)) {
                return resolve(collector);
              }

              if (_.isEmpty(filterObject.date) && _.isEmpty(filterObject.region||{}) && !filterObject.data_series_id) {
                return resolve(collector);
              } else {
                filterObject.collector_id = collectorResult.id;

                return self.addFilter(filterObject, options)
                  .then(function(filterResult) {
                    collector.filter = filterResult;
                    return resolve(collector);
                  }).catch(function(err) {
                    logger.error(err);
                    return reject(new exceptions.CollectorError("Could not save collector filter: " + err.toString()));
                  });
              }
            }).catch(function(err) {
              logger.error(err);
              return reject(new exceptions.CollectorError("Could not save collector input/output " + err.toString()));
            });
        });
      }).catch(function(err) {
        logger.error(err);
        return reject(new exceptions.CollectorError("Could not save collector: ", err));
      });
    });
  },

  /**
   * It performs multiple update collector from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} values - A collector object to update
   * @param {Object} extra - A extra query options like fields
   * @param {Array<string>} extra.fields - An array of field to update
   * @param {Object} options - A orm options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  updateCollectors: function(restriction, values, extra, options) {
    return new Promise(function(resolve, reject) {
      var opts = Utils.extend(Object.assign({
        where: restriction
      }, extra instanceof Object ? extra : {}), options);

      models.db.Collector.update(values, opts).then(function() {
        return resolve();
      }).catch(function(err) {
        logger.error(err);
        return reject(new exceptions.CollectorError("Could not update collector " + err.toString()));
      });
    });
  },

  /**
   * It performs update collector from identifier
   *
   * @param {number} collectorId - A collector identifier
   * @param {Object} collectorObject - A collector object values to update
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  updateCollector: function(collectorId, collectorObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var fields = [
        'service_instance_id',
        'data_series_input',
        'data_series_output',
        'schedule_id',
        'schedule_type',
        'active',
        'collector_type'
      ];

      self.updateCollectors({id: collectorId}, collectorObject, {fields: fields}, options).then(function() {
        return resolve();
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  /**
   * It retrieves all collector input output matches
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Array<Object>>}
   */
  listCollectorInputOutput: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.CollectorInputOutput.findAll(Utils.extend({where: restriction || {}}, options))
        .then(function(inputOutputResult) {
          var output = [];
          inputOutputResult.forEach(function(element) {
            output.push(element.get());
          });
          return resolve(output);
        }).catch(function(err) {
          return reject(err);
        });
    });
  },

  /**
   * It retrieves all collectors from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Array<Object>>}
   */
  listCollectors: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var dataProviderRestriction = {};
      if (restriction && restriction.DataProvider) {
        dataProviderRestriction = restriction.DataProvider;
        delete restriction.DataProvider;
      }

      return models.db.Collector.findAll({
        where: restriction,
        include: [
          models.db.Schedule,
          models.db.CollectorInputOutput,
          {
            model: models.db.Filter,
            required: false,
            attributes: [
              "id",
              "frequency",
              "frequency_unit",
              "discard_before",
              "discard_after",
              "by_value",
              "crop_raster",
              "collector_id",
              "data_series_id",
              [orm.fn('ST_AsEwkt', orm.col('region')), 'region_wkt'],
              [orm.fn('ST_AsGeoJSON', orm.col('region'), 15, 2), 'region']
            ]
          },
          {
            model: models.db.Intersection,
            required: false
          },
          {
            model: models.db.DataSeries,
            include: {
              model: models.db.DataProvider,
              where: dataProviderRestriction
            }
          }
        ]
      })

      .then(function(collectorsResult) {
        var output = [];

        var promises = [];
        collectorsResult.forEach(function(collector) {
          promises.push(self.getDataSeries({id: collector.data_series_output}));
        });

        return Promise.all(promises).then(function(dataSeriesArray) {
          dataSeriesArray.forEach(function(dataSeries) {
            collectorsResult.some(function(collector) {

              if(collector.Filter !== undefined && collector.Filter !== null &&
              collector.Filter.dataValues.region !== undefined && collector.Filter.dataValues.region !== null &&
              typeof collector.Filter.dataValues.region === "string") {
                collector.Filter.dataValues.region = JSON.parse(collector.Filter.dataValues.region);
              }

              if (collector.data_series_output === dataSeries.id) {
                var collectorInstance = new DataModel.Collector(collector.get());
                collectorInstance.dataSeriesOutput = dataSeries;
                output.push(collectorInstance);

                return true;
              }
            });
          });

          return resolve(output);
        }).catch(function(err) {
          logger.error(err);
          return reject(new exceptions.CollectorError("Could not retrieve collector data series output: " + err.toString()));
        });
      }).catch(function(err) {
        logger.error(err);
        return reject(new exceptions.CollectorError("Could not retrieve collector: " + err.message));
      });
    });
  },

  /**
   * It retrieves a collector of database from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Collector>}
   */
  getCollector: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var restrictionOutput = {};
      if (restriction.output) {
        Object.assign(restrictionOutput, restriction.output);
        delete restriction.output;
      }

      models.db.Collector.findOne(Utils.extend({
        where: restriction,
        include: [
          {
            model: models.db.Schedule
          },
          {
            model: models.db.DataSeries,
            where: restrictionOutput
          },
          {
            model: models.db.CollectorInputOutput
          },
          {
            model: models.db.Filter,
            required: false,
            attributes: { include: [[orm.fn('ST_AsEwkt', orm.col('region')), 'region_wkt'], [orm.fn('ST_srid', orm.col('region')), 'srid']] }
          },
          {
            model: models.db.Intersection,
            required: false
          }
        ]
      }, options)).then(function(collectorResult) {
        if (collectorResult) {
          var collectorInstance = new DataModel.Collector(collectorResult.get());

          return self.getDataSeries({id: collectorResult.data_series_output})
            .then(function(dataSeries) {
              collectorInstance.dataSeriesOutput = dataSeries;
              return resolve(collectorInstance);
            }).catch(function(err) {
              logger.error("Retrieved null while getting collector", err);
              return reject(new exceptions.CollectorError("Could not find collector. " + err.toString()));
            });
        } else {
          logger.error("Retrieved null while getting collector", collectorResult);
          return reject(new exceptions.CollectorErrorNotFound("Could not find collector. "));
        }
      }).catch(function(err) {
        logger.error(err);
        return reject(new exceptions.CollectorError("Could not find collector. " + err.toString()));
      });
    });
  },

  /**
   * It retrieves a list of filter in database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  listFilters: function(restriction, options){
    return new Promise(function(resolve, reject) {
      models.db.Filter.findAll({where: restriction}).then(function(filtersResult) {
        var output = [];
        filtersResult.forEach(function(element) {
          output.push(element.get());
        });
        resolve(output);
      }).catch(function(err) {
        reject(new Error("Could not retrieve filter " + err.toString()));
      });
    });

  },

  /**
   * It performs a save intersection in database. It accepts multiple insertions
   *
   * @param {Array<Object>} intersectionArray - An javascript array with intersection values
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Array<DataModel.Intersection>>}
   */
  addIntersection: function(intersectionArray, options) {
    return new Promise(function(resolve, reject) {
      models.db.Intersection.bulkCreate(intersectionArray, Utils.extend({returning: true}, options))
        .then(function(intesectionList) {
          var output = [];
          intesectionList.forEach(function(element) {
            output.push(new DataModel.Intersection(element.get()));
          });
          return resolve(output);
        }).catch(function(err) {
          return reject(new Error("Could not save intersection. " + err.toString()));
        });
    });
  },

  /**
   * It performs update intersection from identifier
   *
   * @param {number} intersectionId - An intersection identifier
   * @param {Object} intersectionObject - An intersection object to update
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
  */
  updateIntersection: function(intersectionId, intersectionObject, options) {
    return new Promise(function(resolve, reject) {
      models.db.Intersection.update(intersectionObject, Utils.extend({
        fields: ['attribute'],
        where: {
          id: intersectionId
        }
      }, options))
        .then(function() {
          return resolve();
        }).catch(function(err) {
          return reject(new Error("Could not update intersection " + err.toString()));
        });
    });
  },

  updateIntersections: function(intersectionIds, intersectionList, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var promises = [];
      intersectionList.forEach(function(element, index) {
        promises.push(self.updateIntersection(intersectionIds[index], element, options));
      });
      Promise.all(promises).then(function() {
        return resolve();
      }).catch(function(err) {
        return reject(err);
      });
    });
  },

  listIntersections: function(restriction) {
    return new Promise(function(resolve, reject) {
      models.db.Intersection.findAll(restriction).then(function(intersectionResult) {
        var output = [];
        intersectionResult.forEach(function(element) {
          output.push(element.get());
        });
        resolve(output);
      }).catch(function(err) {
        reject(new Error("Could not retrieve intersection " + err.toString()));
      });
    });
  },
  /**
   * It performs remove intersection in database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   */
  removeIntersection: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.Intersection.destroy(Utils.extend({where: restriction}, options)).then(function() {
        return resolve();
      }).catch(function(err) {
        return reject(new Error("Could not remove intersection " + err.toString()));
      });
    });
  },

  /**
   * It performs save filter in database
   *
   * @param {Object} filterObject - A filter object values to save
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Filter>}
  */
  addFilter: function(filterObject, options) {
    var self = this;

    return new Promise(function(resolve, reject) {
      var filterValues = {collector_id: filterObject.collector_id, region: filterObject.region || null};

      Object.assign(filterValues, _processFilter(filterObject));

      // checking filter
      models.db.Filter.create(filterValues, options).then(function(filterResult) {
        if (filterResult.region) {
          return self.getFilter({id: filterResult.id}, options).then(function(filterEwkt) {
            return resolve(filterEwkt);
          });
        } else {
          return resolve(new DataModel.Filter(filterResult.get()));
        }
      }).catch(function(err) {
        // todo: improve error message
        logger.error(err);
        return reject(new Error("Could not save filter. " + err.toString()));
      });
    });
  },

  /**
   * It retrieves a filter instance of database from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Filter>}
   */
  getFilter: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.Filter.findOne(Utils.extend({
        attributes: {include: [[orm.fn('ST_AsEwkt', orm.col('region')), 'region_wkt']]},
        where: restriction
      }, options)).then(function(filter) {
        if (filter === null) {
          return reject(new exceptions.FilterError("Could not get filter, retrieved null"));
        }

        var output = new DataModel.Filter(filter.get());
        output.region_wkt = filter.dataValues.region_wkt;

        return resolve(output);
      }).catch(function(err) {
        return reject(new exceptions.FilterError("Could not retrieve filter " + err.toString()));
      });
    });
  },

  /**
   * It performs an update filter from given restriction
   *
   * @param {number} filterId - A filter identifier
   * @param {Object} filterObject - A filter values to update
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  updateFilter: function(filterId, filterObject, options) {
    return new Promise(function(resolve, reject) {
      var filterValues = _processFilter(filterObject);
      return models.db.Filter.update(filterValues, Utils.extend({
        fields: ['frequency', 'frequency_unit', 'discard_before', 'discard_after', 'region', 'by_value', 'crop_raster', 'data_series_id'],
        where: {
          id: filterId
        }
      }, options)).then(function() {
        return resolve();
      }).catch(function(err) {
        logger.error(err);
        return reject(new Error("Could not update filter " + err.toString()));
      });
    });
  },


  /**
   * It performs save analysis data series in database
   *
   * @param {Object} analysisDataSeriesObject - An analysis data series object to save
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<AnalysisDataSeries>}
   */
  addAnalysisDataSeries: function(analysisDataSeriesObject, options) {
    return new Promise(function(resolve, reject) {
      models.db.AnalysisDataSeries.create(analysisDataSeriesObject, Utils.extend({
        include: [models.db.AnalysisDataSeriesMetadata]
      }, options))
        .then(function(result) {
          var output = new DataModel.AnalysisDataSeries(result);
          return resolve(output);
        }).catch(function(err) {
          return reject(new exceptions.AnalysisError("Could not save analysis data series " + err.toString()));
        });
    });
  },

  /**
   * It performs save analysis output grid in database
   *
   * @param {Object} analysisOutputGridObject - An analysis output grid values to save
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<AnalysisOutputGrid>}
   */
  addAnalysisOutputGrid: function(analysisOutputGridObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.AnalysisOutputGrid.create(analysisOutputGridObject, options)
        .then(function(analysisOutGridResult) {
          return self.getAnalysisOutputGrid({id: analysisOutGridResult.id}, options)
            .then(function(outputGrid) {
              return resolve(outputGrid);
            });
      }).catch(function(err) {
        logger.error(err);
        return reject(new exceptions.AnalysisError("Could not save analysis output grid " + err.toString()));
      });
    });
  },

  /**
   * It retrieves a analysis output grid from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<AnalysisOutputGrid>}
   */
  getAnalysisOutputGrid: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.AnalysisOutputGrid.findOne(Utils.extend({
        attributes: {include: [[orm.fn('ST_AsEwkt', orm.col('area_of_interest_box')), 'interest_box']]},
        where: restriction
      }, options)).then(function(outputGrid) {
        if (outputGrid === null) {
          return reject(new Error("Analysis output grid not found"));
        }

        var output = new DataModel.AnalysisOutputGrid(outputGrid.get());
        output.areaOfInterestBoxWKT = outputGrid.dataValues.interest_box;
        return resolve(output);
      }).catch(function(err) {
        logger.error(err);
        return reject(new Error("Analysis output grid not found " + err.toString()));
      });
    });
  },

  /**
   * It performs save reprocessing historical data from analysis identifier
   *
   * @param {number} analysisId - An analysis identifier
   * @param {Object} historicalObject - A historical object value
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Analysis>}
   */
  addHistoricalData: function(analysisId, historicalObject, options) {
    return new Promise(function(resolve, reject) {
      // setting analysis_id in historical Data
      historicalObject.analysis_id = analysisId;

      models.db.ReprocessingHistoricalData.create(historicalObject, options)
        .then(function(historicalResult) {
          return resolve(new DataModel.ReprocessingHistoricalData(historicalResult.get()));
        })
        .catch(function(err) {
          return reject(new Error("Could not save reprocessing historical data " + err.toString()));
        });
    });
  },

  /**
   * It performs update reprocessing historical data from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} historicalObject - A historical object value
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Analysis>}
   */
  updateHistoricalData: function(restriction, historicalObject, options) {
    return new Promise(function(resolve, reject) {
      var opts = Object.assign({
        where: restriction,
        fields: ['startDate', 'endDate']
      }, options);
      models.db.ReprocessingHistoricalData.update(historicalObject, opts)
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(new Error("Could not update reprocessing historical data " + err.toString()));
        });
    });
  },

  /**
   * It performs remove reprocessing historical data from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Analysis>}
   */
  removeHistoricalData: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      models.db.ReprocessingHistoricalData.destroy(Utils.extend({where: restriction}, options))
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(new Error("Could not remove reprocessing historical data"));
        });
    });
  },

  /**
   * It performs save analysis metadata in database
   *
   * @param {Object} analysisMetadataObject - An analysis metadata values to save
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Object>}
   */
  addAnalysisMetadata: function(analysisMetadataObject, options) {
    return new Promise(function(resolve, reject) {
      return models.db.AnalysisMetadata.bulkCreate(analysisMetadataObject, options)
        .then(function(bulkAnalysisMetadata) {
          return resolve(Utils.formatMetadataFromDB(bulkAnalysisMetadata));
        })
        .catch(function(err) {
          return reject(new Error(Utils.format("Could not save analysis metadata due ", err.toString())));
        });
    });
  },

  /**
   * It performs save analysis in database
   *
   * @param {Object} analysisObject - An analysis values to save
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Analysis>}
   */
  addAnalysis: function(analysisObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var analysisResult;
      var dataSet;
      var dataSeries;
      var scheduleResult;
      var historicalData;
      var scriptLanguageResult;
      models.db.Analysis.create(analysisObject, options)
        // successfully creating analysis
        .then(function(analysis) {
          analysisResult = analysis;
          return self.getDataSet({id: analysisResult.dataset_output});
        })
        // successfully retrieving analysis data set
        .then(function(dataSetResult) {
          dataSet = dataSetResult;
          return self.getDataSeries({id: dataSet.data_series_id});
        })
        // successfully retrieving analysis data series
        .then(function(dataSeriesResult) {
          dataSeries = dataSeriesResult;
          var schedulePromise;
          if (analysisResult.schedule_type == Enums.ScheduleType.AUTOMATIC){
            schedulePromise = self.getAutomaticSchedule({id: analysisResult.automatic_schedule_id}, options);
          } else if (analysisResult.schedule_type == Enums.ScheduleType.SCHEDULE || analysisResult.schedule_type == Enums.ScheduleType.REPROCESSING_HISTORICAL){
            schedulePromise = self.getSchedule({id: analysisResult.schedule_id}, options);
          } else {
            schedulePromise = Promise.resolve();
          }
          return schedulePromise;
        })
        // successfully retrieving analysis schedule
        .then(function(schedule) {
          scheduleResult = schedule;
          return analysisResult.getScriptLanguage();
        })
        // successfully retrieving analysis script language
        .then(function(scriptLanguage) {
          scriptLanguageResult = scriptLanguage;
          // checking if there is historical data to save
          if (_.isEmpty(analysisObject.historical) || (!analysisObject.historical.startDate || !analysisObject.historical.endDate)) {
            return null;
          }
          return self.addHistoricalData(analysisResult.id, analysisObject.historical, options);
        })
        // successfully saving reprocessing historical result or just skipping it. Remember it may be null.
        .then(function(historicalResult) {
          historicalData = historicalResult;

          // creating a variable to make visible in closure
          var analysisDataSeriesArray = Utils.clone(analysisObject.analysisDataSeries);
          // making analysis metadata
          var analysisMetadata = [];
          for(var key in analysisObject.metadata) {
            if (analysisObject.metadata.hasOwnProperty(key)) {
              analysisMetadata.push({
                analysis_id: analysisResult.id,
                key: key,
                value: analysisObject.metadata[key]
              });
            }
          }

          return self.addAnalysisMetadata(analysisMetadata, options).then(function(analysisMetadataOutput) {
            var promises = [];

            analysisDataSeriesArray.forEach(function(analysisDS) {
              analysisDS.analysis_id = analysisResult.id;
              // ignore id
              delete analysisDS.id;

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

              promises.push(self.addAnalysisDataSeries(analysisDS, options));
            });

            var analysisInstance = new DataModel.Analysis(analysisResult);
            analysisInstance.setScriptLanguage(scriptLanguageResult);
            if (analysisResult.schedule_type == Enums.ScheduleType.AUTOMATIC){
              analysisInstance.setAutomaticSchedule(scheduleResult);
            } else if(analysisResult.schedule_type == Enums.ScheduleType.SCHEDULE || analysisResult.schedule_type == Enums.ScheduleType.REPROCESSING_HISTORICAL){
              analysisInstance.setSchedule(scheduleResult);
            }
            analysisInstance.setMetadata(analysisMetadataOutput);
            analysisInstance.setDataSeries(dataSeries);

            analysisResult.getAnalysisType().then(function(analysisTypeResult) {
              analysisInstance.type = analysisTypeResult.get();

              var _savePromises = function() {
                return Promise.all(promises).then(function(results) {
                  results.forEach(function(result) {
                    var analysisDataSeries = result;

                    analysisInstance.addAnalysisDataSeries(analysisDataSeries);
                  });

                  return resolve(analysisInstance);
                }).catch(function(err) {
                  logger.error(err);
                  // rollback analysis data series
                  Utils.rollbackPromises([
                    self.removeAnalysis({id: analysisResult.id}, options)
                  ], new exceptions.AnalysisError("Could not save analysis data series " + err.toString()), reject);
                });
              };

              // check for add analysis grid
              if (analysisTypeResult.id === Enums.AnalysisType.GRID) {
                var analysisOutputGrid = analysisObject.grid;
                analysisOutputGrid.analysis_id = analysisResult.id;

                return self.addAnalysisOutputGrid(analysisOutputGrid, options).then(function(output_grid) {
                  analysisInstance.outputGrid = output_grid;
                  return _savePromises();
                }).catch(function(err) {
                  Utils.rollbackPromises([
                    self.removeAnalysis({id: analysisResult.id}, options)
                  ], err, reject);
                });
              } else { return _savePromises(); } // end if
            }); // end get analysis type
          }).catch(function(err) {
            // rollback analysis metadata
            Utils.rollbackPromises([
              self.removeAnalysis({id: analysisResult.id}, options)
            ], new exceptions.AnalysisError("Could not save analysis metadata " + err.toString()), reject);
          });
        })

        .catch(function(err) {
          // rollback data series
          logger.error(err);
          return reject(new exceptions.AnalysisError("Could not save analysis " + err.toString()));
        });
    });
  },
  /**
   * It performs a update analysis from given restriction
   *
   * @param {number} analysisId - An analysis identifier
   * @param {Object} analysisObject - An analysis object to update
   * @param {Object} analysisObject.historical - Reprocessing historical data values
   * @param {Object} scheduleObject - A schedule object to update
   * @param {Object} storagerObject - A storager object to update
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  updateAnalysis: function(analysisId, analysisObject, scheduleObject, storagerObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var analysisInstance;
      /**
       * It is important to pass options to all DB Operations even when options is null|undefined, since
       * it may contain Sequelize.Transaction object to handle entire operation. If transaction was passed but
       * not applied on children operations, Your DB operation may be inconsistent.
       */
      // Retrieve analysis and update Analysis Table
      var removeSchedule;
      var scheduleIdToRemove;
      var scheduleTypeToRemove;
      return self.getAnalysis({id: analysisId}, options).then(function(analysisResult) {
        analysisInstance = analysisResult;
        var oldScheduleType = analysisResult.scheduleType;
        var newScheduleType = scheduleObject.scheduleType;
        var schedulePromise;
        /**
         * Check if must just update the schedule if the schedule type don't change, or if must delete a schedule and create a new one
         */
        // If don't change the type update the schedule
        if (oldScheduleType == newScheduleType){
          if (newScheduleType == Enums.ScheduleType.SCHEDULE || newScheduleType == Enums.ScheduleType.REPROCESSING_HISTORICAL){
            return self.updateSchedule(analysisInstance.schedule.id, scheduleObject, options);
          } else if (newScheduleType == Enums.ScheduleType.AUTOMATIC){
            return self.updateAutomaticSchedule(analysisInstance.automaticSchedule.id, scheduleObject, options);
          }
        } else if ((newScheduleType == Enums.ScheduleType.SCHEDULE && oldScheduleType == Enums.ScheduleType.REPROCESSING_HISTORICAL) ||
                    (oldScheduleType == Enums.ScheduleType.SCHEDULE && newScheduleType == Enums.ScheduleType.REPROCESSING_HISTORICAL) ){
            return self.updateSchedule(analysisInstance.schedule.id, scheduleObject, options);
        } else {
          // If don't have a schedule previously, create a new one
          if (oldScheduleType == Enums.ScheduleType.MANUAL){
            return self.addSchedule(scheduleObject, options).then(function(newSchedule){
              if (newScheduleType == Enums.ScheduleType.SCHEDULE || newScheduleType == Enums.ScheduleType.REPROCESSING_HISTORICAL){
                analysisObject.schedule_id = newSchedule.id;
                analysisResult.schedule = newSchedule;
                return null;
              } else {
                analysisObject.automatic_schedule_id = newSchedule.id;
                analysisResult.automaticSchedule = newSchedule;
                return null;
              }
            });
            // If old schedule is automatic, remove the schedule
          } else if (oldScheduleType == Enums.ScheduleType.AUTOMATIC){
            removeSchedule = true;
            scheduleIdToRemove = analysisResult.automaticSchedule.id;
            scheduleTypeToRemove = oldScheduleType;
            analysisObject.automatic_schedule_id = null;
            // new schedule is not MANUAL create and set the id in analysis object to update
            if (newScheduleType == Enums.ScheduleType.SCHEDULE || newScheduleType == Enums.ScheduleType.REPROCESSING_HISTORICAL){
              return self.addSchedule(scheduleObject, options).then(function(newSchedule){
                analysisObject.schedule_id = newSchedule.id;
                analysisResult.schedule = newSchedule;
                return null;
              });
            } else {
              return null;
            }
            // when the old schedule is SCHEDULE or REPROCESSING HISTORICAL, remove the schedule
          } else {
            removeSchedule = true;
            scheduleIdToRemove = analysisResult.schedule.id;
            scheduleTypeToRemove = oldScheduleType;
            analysisObject.schedule_id = null;
            if (newScheduleType == Enums.ScheduleType.AUTOMATIC){
              return self.addSchedule(scheduleObject, options).then(function(newSchedule){
                analysisObject.automatic_schedule_id = newSchedule.id;
                analysisResult.automaticSchedule = newSchedule;
                return null;
              });
            } else {
              return null;
            }
          }
        }
      })
      .then(function (){
        return models.db.Analysis.update(analysisObject, Utils.extend({
          fields: ['name', 'description', 'instance_id', 'script', 'active', 'schedule_type', 'schedule_id', 'automatic_schedule_id'],
          where: {
            id: analysisId
          }
        }, options));
      })

      // Prepare to update Analysis Dependencies
      .then(function() {
        // updating analysis dataSeries
        var promises = [];

        var toUpdate = analysisObject.analysisDataSeries.filter(function(elm) { return elm.id !== 0; });
        var toRemove = _.differenceWith(analysisInstance.analysis_dataseries_list, toUpdate, function(a, b) {
          return a.id === b.id;
        });

        // TODO: improve this and delete
        analysisObject.analysisDataSeries.some(function(element) {
          if (element.id && element.id > 0) {
            // update
            promises.push(models.db.AnalysisDataSeries.update(element, Utils.extend({
              fields: ['alias'],
              where: {id: element.id}
            }, options)));

            // update analysis data series metadata
            if (!Utils.isEmpty(element.metadata || {})) {
              var dsMetaArr = Utils.generateArrayFromObject(element.metadata, function(key, value, analysisDsId) {
                return {"key": key, "value": value, "analysis_data_series_id": analysisDsId};
              }, element.id);
              promises.push(models.db.AnalysisDataSeriesMetadata.update(dsMetaArr[0], Utils.extend({
                fields: ['key', 'value'],
                where: {analysis_data_series_id: element.id}
              }, options)));
            }
          } else {
            // insert
            element.analysis_id = analysisInstance.id;
            delete element.id;
            promises.push(self.addAnalysisDataSeries(element, options));
          }
        });

        // delete
        toRemove.forEach(function(element) {
          promises.push(models.db.AnalysisDataSeries.destroy(Utils.extend({where: {
            id: element.id
          }}, options)));
        });

        return Promise.all(promises);
      })
      // check if have to remove a schedule
      .then(function() {
        // remove schedule
        if (removeSchedule){
          if (scheduleTypeToRemove == Enums.ScheduleType.AUTOMATIC){
            return DataManager.removeAutomaticSchedule({id: scheduleIdToRemove}, options);
          } else {
            return DataManager.removeSchedule({id: scheduleIdToRemove}, options);
          }
        }
      })
      // Update historical data if there is
      .then(function() {
        // reprocessing historical data
        if (!_.isEmpty(analysisObject.historical)) {
          // update
          if (analysisInstance.historicalData.id) {
            // setting to null when
            var historicalData = analysisObject.historical;
            if (historicalData.startDate === "") {
              historicalData.startDate = null;
            }
            if (historicalData.endDate === "") {
              historicalData.endDate = null;
            }

            if (!historicalData.endDate && !historicalData.startDate) {
              // delete
              return self.removeHistoricalData({id: analysisInstance.historicalData.id}, options);
            }

            return self.updateHistoricalData({id: analysisInstance.historicalData.id}, historicalData, options);
          } else {
            if (analysisObject.historical.startDate || analysisObject.historical.endDate) {
              // save
              return self.addHistoricalData(analysisInstance.id, analysisObject.historical, options);
            }
          }
        }

        return null;
      })
      // Update Analysis DCP or Grid if there is
      .then(function() {
        switch (analysisInstance.type.id) {
          case Enums.AnalysisType.GRID:
            var gridObject = Object.assign(
              {},
              analysisInstance.outputGrid.toObject ? analysisInstance.outputGrid.toObject() : analysisInstance.outputGrid);
            // reset
            for(var k in gridObject) {
              if (gridObject.hasOwnProperty(k)) {
                gridObject[k] = null;
              }
            }
            Object.assign(gridObject, analysisObject.grid);

            // If no area of interest typed, reset interest box. It is important because when there is no bounded box but there is
            // in database, it will keep, since undefined !== null.
            if (Utils.isEmpty(gridObject.area_of_interest_bounded)) {
              gridObject.area_of_interest_bounded = null;
              gridObject.area_of_interest_box = null;
            }

            return models.db.AnalysisOutputGrid.update(gridObject, Utils.extend({
              fields: ['area_of_interest_box', 'srid', 'resolution_x',
                        'resolution_y', 'interpolation_dummy',
                        'area_of_interest_type', 'resolution_type',
                        'interpolation_method', 'resolution_data_series_id',
                        'area_of_interest_data_series_id'],
              where: {
                id: analysisInstance.outputGrid.id
              }
            }, options));
          case Enums.AnalysisType.MONITORED:
          case Enums.AnalysisType.DCP:
            // save/update
            var newMetadata = Utils.generateArrayFromObject(analysisObject.metadata, function(key, value) {
              return {"key": key, "value": value, "analysis_id": analysisInstance.id};
            });

            /**
             * Promise chain (destroy/save)
             *
             * @type {Promise}
             */
            var promise;
            // checking if there is
            if (Utils.isEmpty(analysisInstance.metadata)) {
              // add
              promise = Promise.resolve();
            } else {
              // remove
              promise = models.db.AnalysisMetadata.destroy(Utils.extend(
                {
                  where: {
                    analysis_id: analysisInstance.id
                  }
                }, options));
            }

            return promise
              .then(function() {
                return self.addAnalysisMetadata(newMetadata, options);
              });
          default:
            // propagate next promise
            return null;
        }
      })
      /**
       * finally, update data series. It is important to put it at end of operations, since the data series are cached.
       * So, if it uses transaction object and an exception occurs, it is not necessary to force data to original state
       */
      .then(function() {
        var dataSeries = Utils.clone(analysisInstance.dataSeries);
        dataSeries.name = analysisObject.name;
        // TODO: change it
        dataSeries.description = "Generated by analysis " + analysisObject.name;
        dataSeries.data_provider_id = analysisObject.data_provider_id;
        dataSeries.dataSets[0].format = storagerObject.format;
        dataSeries.data_series_semantics_id = dataSeries.data_series_semantics.id;
        return self.updateDataSeries(analysisInstance.dataSeries.id, dataSeries, options);
      })

      .then(function() {
        return resolve();
      })

      .catch(function(err) {
        return reject(new exceptions.AnalysisError("Could not update analysis " + err.toString()));
      });
    });
  },

  /**
   * It retrieves all analysis data series in database from given restriction
   *
   * @param {Object} restriction - An analysis data series restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  listAnalysisDataSeries: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.AnalysisDataSeries.findAll(Utils.extend({
        where: restriction || {}
      }, options)).then(function(analysisDataSeriesResult){
        return resolve(analysisDataSeriesResult.map(function(analysisDataSeries){
          return new DataModel.AnalysisDataSeries(analysisDataSeries.get());
        }));
      });
    });
  },

  /**
   * It retrieves all analysis in database from given restriction
   *
   * @param {Object} restriction - An analysis identifier
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  listAnalysis: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var _reject = function(err) {
        logger.error(err);
        reject(err);
      };

      return models.db.Analysis.findAll(Utils.extend({
        include: [
          {
            model: models.db.AnalysisDataSeries,
            include: [
              {
                model: models.db.AnalysisDataSeriesMetadata,
                required: false
              }
            ]
          },
          {
            model: models.db.AnalysisOutputGrid,
            attributes: [
              'interpolation_dummy',
              'resolution_x',
              'resolution_y',
              'srid',
              'analysis_id',
              'area_of_interest_type',
              'resolution_type',
              'interpolation_method',
              'resolution_data_series_id',
              'area_of_interest_data_series_id',
              [orm.fn('ST_AsGeoJSON', orm.col('area_of_interest_box'), 15, 2), 'area_of_interest_box'],
              [orm.fn('ST_AsEwkt', orm.col('area_of_interest_box')), 'interest_box'] // extra field
            ],
            required: false
          },
          {
            model: models.db.ReprocessingHistoricalData,
            required: false
          },
          models.db.AnalysisMetadata,
          models.db.ScriptLanguage,
          models.db.AnalysisType,
          models.db.Schedule,
          models.db.AutomaticSchedule
        ],
        where: restriction || {}
      }, options)).then(function(analysisResult) {
        var output = [];
        var promises = [];

        analysisResult.forEach(function(analysis) {
          promises.push(self.getDataSet({id: analysis.dataset_output}));
        });

        return Promise.all(promises).then(function(dataSets) {
          promises = [];

          dataSets.forEach(function(dataSet) {
            promises.push(self.getDataSeries({id: dataSet.data_series_id}));
          });

          return Promise.all(promises).then(function(dataSeriesList) {
            analysisResult.forEach(function(analysis) {
              var analysisObject = new DataModel.Analysis(analysis.get());

              dataSeriesList.some(function(dataSeries) {
                return dataSeries.dataSets.some(function(dSet) {
                  if (analysis.dataset_output === dSet.id) {
                    analysisObject.setDataSeries(dataSeries);
                    return true;
                  }
                });
              });

              analysis.AnalysisDataSeries.forEach(function(analysisDataSeries) {
                analysisObject.addAnalysisDataSeries(new DataModel.AnalysisDataSeries(analysisDataSeries.get()));
              });

              output.push(analysisObject);
            });

            return resolve(output);
          }).catch(_reject);
        }).catch(_reject);
      }).catch(_reject);
    });
  },
  /**
   * It retrieve a TerraMA² Analysis instance.
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} restriction.dataSet - TerraMA² Output data set restriction
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @param {boolean} ignoreAnalysisDsMetaDataSeries - Flag that indicates if the AnalysisDsMetaDataSeries should be ignored
   * @return {Promise<DataModel.Analysis>}
   */
  getAnalysis: function(restriction, options, ignoreAnalysisDsMetaDataSeries) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var restrict = Object.assign({}, restriction || {});
      var dataSetRestriction = {};
      if (restrict && restrict.dataSet) {
        dataSetRestriction = restrict.dataSet;
        delete restrict.dataSet;
      }
      var opts = Utils.extend({
        where: restrict,
        include: [
          {
            model: models.db.AnalysisDataSeries,
            include: [
              {
                model: models.db.AnalysisDataSeriesMetadata,
                required: false
              }
            ]
          },
          {
            model: models.db.AnalysisOutputGrid,
            attributes: {include: [[orm.fn('ST_AsEwkt', orm.col('area_of_interest_box')), 'interest_box']]},
            required: false
          },
          {
            model: models.db.ReprocessingHistoricalData,
            required: false
          },
          models.db.AnalysisMetadata,
          models.db.ScriptLanguage,
          models.db.AnalysisType,
          {
            model: models.db.Schedule,
            required: false
          },
          {
            model: models.db.AutomaticSchedule,
            required: false
          },
          {
            model: models.db.DataSet,
            where: dataSetRestriction
          }
        ]
      }, options);

      return models.db.Analysis.findOne(opts).then(function(analysisResult) {
        var analysisInstance = new DataModel.Analysis(analysisResult.get());

        return self.getDataSet({id: analysisResult.dataset_output}).then(function(analysisOutputDataSet) {
          return self.getDataSeries({id: analysisOutputDataSet.data_series_id}).then(function(analysisOutputDataSeries) {
            analysisInstance.setDataSeries(analysisOutputDataSeries);
            analysisResult.AnalysisDataSeries.forEach(function(analysisDataSeries) {
              var ds = Utils.find(self.data.dataSeries, {id: analysisDataSeries.data_series_id});
              var analysisDsMeta = new DataModel.AnalysisDataSeries(analysisDataSeries.get());
              if(ignoreAnalysisDsMetaDataSeries == undefined || ignoreAnalysisDsMetaDataSeries == null || !ignoreAnalysisDsMetaDataSeries) analysisDsMeta.setDataSeries(ds);
              analysisInstance.addAnalysisDataSeries(analysisDsMeta);
            });

            return resolve(analysisInstance);
          }).catch(function(err) {
            return reject(err);
          });
        }).catch(function(err) {
          return reject(err);
        });
      }).catch(function(err) {
        logger.error(err);
        return reject(new exceptions.AnalysisError("Could not retrieve Analysis " + err.message));
      });
    });
  },

  /**
   * It removes Analysis from param. It should be an object containing either id identifier or name identifier.
   *
   * @param {Object} analysisParam - An object containing Analysis identifier to get it.
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise} - a 'bluebird' module with Analysis instance or error callback
   */
  removeAnalysis: function(analysisParam, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return self.getAnalysis({id: analysisParam.id}, options).then(function(analysisResult) {
        return models.db.Analysis.destroy(Utils.extend({where: {id: analysisParam.id}}, options)).then(function() {
          return self.removeDataSerie({id: analysisResult.dataSeries.id}, options).then(function() {
            var removeSchedulePromise;
            if (analysisResult.scheduleType == Enums.ScheduleType.SCHEDULE || analysisResult.scheduleType == Enums.ScheduleType.REPROCESSING_HISTORICAL){
              removeSchedulePromise = self.removeSchedule({id: analysisResult.schedule.id}, options);
            } else if (analysisResult.scheduleType == Enums.ScheduleType.AUTOMATIC ){
              removeSchedulePromise = self.removeAutomaticSchedule({id: analysisResult.automaticSchedule.id}, options);
            } else {
              removeSchedulePromise = Promise.resolve();
            }
            return removeSchedulePromise.then(function() {
              return resolve();
            }).catch(function(err) {
              logger.error("Could not remove analysis schedule ", err);
              return reject(err);
            });
          }).catch(function(err) {
            logger.error("Could not remove output data series ", err);
            return reject(err);
          });
        }).catch(function(err) {
          logger.error(err);
          return reject(new exceptions.AnalysisError("Could not remove Analysis with a collector associated", err));
        });
      }).catch(function(err) {
        logger.error(err);
        return reject(err);
      });
    });
  },

  /**
   * It performs save alert in database
   *
   * @param {Object} alertObject - An alert values to save
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Alert>}
   */
  addAlert: function(alertObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      var alertResult;
      var scheduleResult;
      var reportMetadataResult;
      var additionalDataResult;
      var legendResult;
      var notificationResult;
      return models.db.Alert.create(alertObject, options)
        .then(function(alert){
          alertResult = alert;
          if (alertResult.schedule_id) {
           return self.getSchedule({id: alertResult.schedule_id}, options);
          } else if (alertResult.automatic_schedule_id){
            return self.getAutomaticSchedule({id: alertResult.automatic_schedule_id}, options);
          } else {
            return null;
          }
        })
        .then(function(schedule){
          scheduleResult = schedule;
          var reportMetadata = alertObject.report_metadata;
          reportMetadata.alert_id = alertResult.id;
          return self.addReportMetadata(reportMetadata, options);
        })
        .then(function(reportMetadata){
          reportMetadataResult = reportMetadata;
          var alertAdditionalData = alertObject.additionalData;
          var additionalDataPromises = [];
          if (alertAdditionalData){
            alertAdditionalData.forEach(function(alertAD){
              alertAD.alert_id = alertResult.id;
              additionalDataPromises.push(self.addAlertAdditionalData(alertAD, options));
            });
          }
          return Promise.all(additionalDataPromises);
        })
        .then(function(additionalData){
          additionalDataResult = additionalData;
          return self.getLegend({id: alertResult.legend_id}, options);
        })
        .then(function(legend){
          legendResult = legend;
          var notifications = alertObject.notifications;
          var notificationsPromises = [];
          notifications.forEach(function(notification){
            notification.alert_id = alertResult.id;
            notificationsPromises.push(self.addAlertNotification(notification, options));
          });
          return Promise.all(notificationsPromises);
        })
        .then(function(notifications){
          notificationResult = notifications;
          var objectToAssign = {
            reportMetadata: reportMetadataResult,
            additionalData: additionalDataResult,
            legend: legendResult,
            notifications: notificationResult
          };
          if (alertResult.shedule_id){
            objectToAssign.schedule = scheduleResult || {};
            objectToAssign.automaticSchedule = {}
          } else {
            objectToAssign.schedule = {};
            objectToAssign.automatic_schedule = scheduleResult || {};
          }
          return resolve(new DataModel.Alert(Object.assign(alertResult.get(), objectToAssign)));
        })
        .catch(function(err){
          return reject(new Error(Utils.format("Could not save alert due %s", err.toString())));
        });
    });
  },

  /**
   * It performs save attached view in database
   *
   * @param {Object} attachedViewObject - An attached view values to save
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Alert>}
   */
  addAlertAttachedView: function(attachedViewObject, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.AlertAttachedView.create(attachedViewObject, options).then(function(attachedView) {
        return resolve(new DataModel.AlertAttachedView(Object.assign(attachedView.get(), {})));
      }).catch(function(err){
        return reject(new Error(Utils.format("Could not save attached view due %s", err.toString())));
      });
    });
  },

  /**
   * It performs save alert attachment in database
   *
   * @param {Object} alertAttachmentObject - An alert attachment values to save
   * @param {Object} options - A query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Alert>}
   */
  addAlertAttachment: function(alertAttachmentObject, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.AlertAttachment.create(alertAttachmentObject, options).then(function(alertAttachment) {
        return resolve(new DataModel.AlertAttachment(Object.assign(alertAttachment.get(), {})));
      }).catch(function(err){
        return reject(new Error(Utils.format("Could not save alert attachment due %s", err.toString())));
      });
    });
  },

  /**
   * It performs update attached view from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} alertAttachedViewObject - An attached view object values to update
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   */
  updateAlertAttachedView: function(restriction, alertAttachedViewObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      var alertId = restriction.id;
      models.db.AlertAttachedView.update(
        alertAttachedViewObject,
        Utils.extend({
          fields: ["layer_order", "view_id"],
          where: restriction
        }, options))

        .then(function() {
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not update attached view " + err.toString()));
        });
    })
  },

  /**
   * It performs update alert attachment from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} alertAttachmentObject - An alert attachment object values to update
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   */
  updateAlertAttachment: function(restriction, alertAttachmentObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      var alertId = restriction.id;
      models.db.AlertAttachment.update(
        alertAttachmentObject,
        Utils.extend({
          fields: ["y_max", "y_min", "x_max", "x_min", "srid"],
          where: restriction
        }, options))

        .then(function() {
          self.getAlertAttachment(restriction).then(function(alertAttachment) {
            var alertAttachmentObject = Object.assign(alertAttachment.get(), {});

            if(alertAttachmentObject.AlertAttachedViews[0].View.ServiceInstance.ServiceMetadata) {
              for(var i = 0, serviceMetadataLength = alertAttachmentObject.AlertAttachedViews[0].View.ServiceInstance.ServiceMetadata.length; i < serviceMetadataLength; i++) {
                if(alertAttachmentObject.AlertAttachedViews[0].View.ServiceInstance.ServiceMetadata[i].dataValues.key === "maps_server") {
                  alertAttachmentObject.geoserverUri = alertAttachmentObject.AlertAttachedViews[0].View.ServiceInstance.ServiceMetadata[i].dataValues.value;
                  break;
                }
              }
            }

            return resolve(new DataModel.AlertAttachment(alertAttachmentObject));
          });
        })

        .catch(function(err) {
          return reject(new Error("Could not update alert attachment " + err.toString()));
        });
    })
  },

  /**
   * It removes an attached view from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  removeAlertAttachedView: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.AlertAttachedView.destroy(Utils.extend({where: restriction}, options))
        .then(function(){
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not remove attached view " + err.toString()));
        });
    });
  },

  /**
   * It removes an alert attachment from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  removeAlertAttachment: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.AlertAttachment.destroy(Utils.extend({where: restriction}, options))
        .then(function(){
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not remove alert attachment " + err.toString()));
        });
    });
  },

  /**
   * It performs a save report metadata and retrieve it
   *
   * @param {Object} reportMetadaObject - Report Metadata object to save
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Object>} An Report Metadata object created
   */
  addReportMetadata: function(reportMetadaObject, options){
    return new Promise(function(resolve, reject){
      return models.db.ReportMetadata.create(reportMetadaObject, options)
        .then(function(reportMetadaResult){
          return resolve(reportMetadaResult.get());
        })
        .catch(function(err){
          return reject(new Error(Utils.format("Could not save alert report metadata due %s", err.toString())));
        });
    });
  },

  /**
   * It performs a save additional data and retrieve it
   *
   * @param {Object} additionalDataObject - Additional data object to save
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Object>} An Additional data object created
   */
  addAlertAdditionalData: function(additionalDataObject, options){
    return new Promise(function(resolve, reject){
      return models.db.AlertAdditionalData.create(additionalDataObject, options)
        .then(function(additionalDataResult){
          return resolve(additionalDataResult.get());
        })
        .catch(function(err){
          return reject(new Error(Utils.format("Could not save alert additional data due %s", err.toString())));
        })
    });
  },

  /**
   * It performs a save legend and retrieve it
   *
   * @param {Object} legendObject - Legend object to save
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Object>} A legend object created
   */
  addLegend: function(legendObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      var legendObjectResult;
      return models.db.Legend.create(legendObject, options)
        .then(function(legendResult){
          legendObjectResult = legendResult;
          var legendLevelPromises = [];
          var legendLevels = legendObject.levels;
          legendLevels.forEach(function(legendLevel){
            legendLevel.legend_id = legendResult.id;
            legendLevelPromises.push(self.addLegendLevel(legendLevel, options));
          });
          return Promise.all(legendLevelPromises)
          .then(function(legendLevelResult){
            var objectToAssign = {
              legendLevels: legendLevelResult
            }
            return resolve(new DataModel.Legend(Object.assign(legendObjectResult.get(), objectToAssign)));
          })
        })
        .catch(function(err){
          return reject(new Error(Utils.format("Could not save legend due %s", err.toString())));
        });
    });
  },

  /**
   * It removes an legend from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  removeLegend: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.Legend.destroy(Utils.extend({where: restriction}, options))
        .then(function(){
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not remove legend " + err.toString()));
        });
    });
  },

  /**
   * It performs a save legend level and retrieve it
   *
   * @param {Object} legendLevelObject - Legend level object to save
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Object>} A legend level object created
   */
  addLegendLevel: function(legendLevelObject, options){
    return new Promise(function(resolve, reject){
      return models.db.LegendLevel.create(legendLevelObject, options)
        .then(function(legendLevelResult){
          return resolve(legendLevelResult.get());
        })
        .catch(function(err){
          return reject(new Error(Utils.format("Could not save legend level due %s", err.toString())));
        });
    });
  },

  /**
   * It performs a save alert notification and retrieve it
   *
   * @param {Object} alertNotificationObject - Alert notification object to save
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Object>} A alert notification object created
   */
  addAlertNotification: function(alertNotificationObject, options){
    return new Promise(function(resolve, reject){
      return models.db.AlertNotification.create(alertNotificationObject, options)
        .then(function(alertNotificationResult){
          return resolve(alertNotificationResult.get());
        })
        .catch(function(err){
          return reject(new Error(Utils.format("Could not save Alert Notification due %s", err.toString())));
        });
    })
  },

  /**
   * It retrieves an alert from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.Alert>}
   */
  getAlert: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      self.listAlerts(restriction, options)
        .then(function(alerts) {
          if (alerts.length === 0) {
            return reject(new Error("No alert retrieved"));
          }
          if (alerts.length > 1) {
            return reject(new Error("Get operation retrieved more than a view"));
          }
          return resolve(alerts[0]);
        })
        .catch(function(err) {
          return reject(err);
        });
    });
  },

  /**
   * It retrieves a list of alerts in database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.Alert[]>}
   */
  listAlerts: function(restriction, options){
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.Alert.findAll(Utils.extend({
        where: restriction || {},
        include: [
          {
            model: models.db.AlertAdditionalData
          },
          {
            model: models.db.AlertNotification
          },
          {
            model: models.db.Schedule
          },
          {
            model: models.db.AutomaticSchedule
          },
          {
            model: models.db.ReportMetadata
          },
          {
            model: models.db.AlertAttachment,
            required: false,
            include: [
              {
                model: models.db.AlertAttachedView,
                required: false,
                include: [
                  {
                    model: models.db.View
                  }
                ]
              }
            ]
          },
          {
            model: models.db.Legend,
            include: [
              {
                model: models.db.LegendLevel
              }
            ]
          },
          {
            model: models.db.View,
            include: [
              {
                model: models.db.ServiceInstance,
                include: [
                  {
                    model: models.db.ServiceMetadata,
                    required: false
                  }
                ]
              },
              {
                model: models.db.Schedule,
                required: false
              },
              {
                model: models.db.AutomaticSchedule,
                required: false
              },
              {
                model: models.db.DataSeries,
                include: [
                  {
                    model: models.db.DataProvider
                  },
                  {
                    model: models.db.DataSeriesSemantics
                  }
                ]
              },
              {
                model: models.db.ViewStyleLegend,
                required: false,
                include: [
                  {
                    model: models.db.ViewStyleColor
                  },
                  {
                    model: models.db.ViewStyleLegendMetadata
                  }
                ]
              }
            ]
          },
          {
            model: models.db.DataSeries,
            include: [
              {
                model: models.db.DataProvider
              },
              {
                model: models.db.DataSeriesSemantics
              },
              {
                model: models.db.DataSet
              }
            ]
          }
        ],
        order: [
          [models.db.AlertAttachment, models.db.AlertAttachedView, 'layer_order', 'ASC']
        ]
      }, options))
        .then(function(alerts){
          return resolve(alerts.map(function(alert) {
            var additionalDatas = [];
            alert.AlertAdditionalData.forEach(function(additionalData){
              additionalDatas.push(additionalData.get());
            });

            var notifications = [];
            alert.AlertNotifications.forEach(function(notification){
              notifications.push(notification.get());
            });

            var legend = new DataModel.Legend(alert.Legend.get());

            var view = alert.View ? new DataModel.View(Object.assign(alert.View.get(), {
              schedule: alert.View.Schedule ? new DataModel.Schedule(alert.View.Schedule.get()) : {},
              automaticSchedule: alert.View.AutomaticSchedule ? new DataModel.AutomaticSchedule(alert.View.AutomaticSchedule.get()) : {},
              dataSeries: alert.View.DataSery ? new DataModel.DataSeries(alert.View.DataSery.get()) : {}
            })) : {};

            if (alert.View && alert.View.ViewStyleLegend) {
              var legendModel = new DataModel.ViewStyleLegend(Utils.extend(
                alert.View.ViewStyleLegend.get(), {colors: alert.View.ViewStyleLegend.ViewStyleColors ? alert.View.ViewStyleLegend.ViewStyleColors.map(function(elm) { return elm.get(); }) : []}));
              legendModel.setMetadata(Utils.formatMetadataFromDB(alert.View.ViewStyleLegend.ViewStyleLegendMetadata));
              view.setLegend(legendModel);
            }

            if(alert.AlertAttachment) {
              var alertAttachment = alert.AlertAttachment.get();

              if(alert.View.ServiceInstance.ServiceMetadata) {
                for(var i = 0, serviceMetadataLength = alert.View.ServiceInstance.ServiceMetadata.length; i < serviceMetadataLength; i++) {
                  if(alert.View.ServiceInstance.ServiceMetadata[i].dataValues.key === "maps_server") {
                    alertAttachment.geoserverUri = alert.View.ServiceInstance.ServiceMetadata[i].dataValues.value;
                    break;
                  }
                }
              }
            }

            var alertModel = new DataModel.Alert(Object.assign(alert.get(), {
              schedule: alert.Schedule ? new DataModel.Schedule(alert.Schedule.get()) : {},
              automatic_schedule: alert.AutomaticSchedule ? new DataModel.AutomaticSchedule(alert.AutomaticSchedule.get()) : {},
              additionalData: additionalDatas,
              notifications: notifications,
              reportMetadata: alert.ReportMetadatum.get(),
              legend: legend,
              view: view,
              dataSeries: alert.DataSery ? new DataModel.DataSeries(alert.DataSery.get()) : {},
              attachment: alertAttachment ? new DataModel.AlertAttachment(alertAttachment) : {}
            }));
            return alertModel;
          }))
        })
        .catch(function(err){
          return reject(new Error("Could not list alerts " + err.toString()));
        });
    });
  },

  /**
   * It retrieves a list of attached viewS in database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<[]>}
   */
  listAlertAttachedViews: function(restriction, options){
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.AlertAttachedView.findAll(Utils.extend({
        where: restriction || {},
        order: [
          ['layer_order', 'ASC']
        ],
        include: [
          {
            model: models.db.AlertAttachment,
            include: [
              {
                model: models.db.Alert
              }
            ]
          },
          {
            model: models.db.View,
            include: [
              {
                model: models.db.ServiceInstance,
                include: [
                  {
                    model: models.db.ServiceMetadata
                  }
                ]
              }
            ]
          }
        ]
      }, options)).then(function(alertAttachedViews) {
        return resolve(alertAttachedViews);
      }).catch(function(err){
        return reject(new Error("Could not list attached views " + err.toString()));
      });
    });
  },

  /**
   * It retrieves an alert attachment
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<[]>}
   */
  getAlertAttachment: function(restriction, options){
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.AlertAttachment.findOne(Utils.extend({
        where: restriction || {},
        include: [
          {
            model: models.db.Alert
          },
          {
            model: models.db.AlertAttachedView,
            include: [
              {
                model: models.db.View,
                include: [
                  {
                    model: models.db.ServiceInstance,
                    include: [
                      {
                        model: models.db.ServiceMetadata
                      }
                    ]
                  }
                ]
              }
            ]
          }
        ],
        order: [
          [models.db.AlertAttachedView, 'layer_order', 'ASC']
        ]
      }, options)).then(function(alertAttachment) {
        return resolve(alertAttachment);
      }).catch(function(err){
        return reject(new Error("Could not get alert attachment " + err.toString()));
      });
    });
  },

  /**
   * It retrieves a legend from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.Legend>}
   */
  getLegend: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.Legend.findOne(Utils.extend({
        where: restriction || {},
        include: [
          {
            model: models.db.LegendLevel
          }
        ]
      }, options)).then(function(legend) {
        if (legend) {
          var legendLevels = [];
          legend.LegendLevels.forEach(function(legendLevel){
            legendLevels.push(legendLevel.get());
          });
          var legendObject = new DataModel.Legend(Object.assign(legend.get(),{
            legendLevels: legendLevels
          }));
          return resolve(legendObject);
        }
        return reject(new Error("Could not find legend"));
      });
    });
  },

  /**
   * It retrieves a list of legends in database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.Legend[]>}
   */
  listLegends: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject){
      models.db.Legend.findAll(Utils.extend({
        where: restriction || {},
        include: [
          {
            model: models.db.LegendLevel
          }
        ]
      }, options))
        .then(function(legends){
          return resolve(legends.map(function(legend){
            var legendLevels = [];
            legend.LegendLevels.forEach(function(legendLevel){
              legendLevels.push(legendLevel.get());
            });
            var legendModel = new DataModel.Legend(Object.assign(legend.get(), {
              legendLevels: legendLevels
            }));
            return legendModel;
          }))
        })
        .catch(function(err){
          return reject(new Error("Could not list legends " + err.toString()));
        });
    });
  },

  /**
   * It performs update alerts from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} alertObject - An alert object values to update
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   */
  updateAlert: function(restriction, alertObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      var alertId = restriction.id;
      models.db.Alert.update(
        alertObject,
        Utils.extend({
          fields: ["name", "description", "data_series_id", "active", "service_instance_id", "legend_id", "legend_attribute", "schedule_type", "schedule_id", "automatic_schedule_id", "view_id"],
          where: restriction
        }, options))
        .then(function(){
          return self.updateReportMetadata({alert_id: alertId}, alertObject.report_metadata, options)
        })

        .then(function() {
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not update alert " + err.toString()));
        });
    })
  },

  /**
   * It performs update report metadata from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} alertObject - An alert report metadata values to update
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   */
  updateReportMetadata: function(restriction, reportMetadataObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      models.db.ReportMetadata.update(
        reportMetadataObject,
        Utils.extend({
          fields: ['title', 'abstract', 'description', 'author', 'contact', 'copyright', 'timestamp_format', 'logo_path', 'document_format'],
          where: restriction
        }, options))

        .then(function(reportMetadata) {
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not update Report metadata " + err.toString()));
        });
    })
  },

  /**
   * It performs update legend from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} alertObject - A legend values to update
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   */
  updateLegend: function(restriction, legendObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      return self.getLegend({id: restriction.id}, options)
        // Updating Legend Levels
        .then(function(legendResult){
          var oldLegendLevels = legendResult.levels;
          var newLegendLevels = legendObject.levels;
          var legendLevelPromises = [];
          newLegendLevels.forEach(function(legendLevel){
            if (legendLevel.id){
              legendLevelPromises.push(self.updateLegendLevel({id: legendLevel.id}, legendLevel, options));
            }
            else {
              legendLevel.legend_id = restriction.id;
              legendLevelPromises.push(self.addLegendLevel(legendLevel, options));
            }
          });
          oldLegendLevels.forEach(function(legendLevel){
            var found = newLegendLevels.some(function(newLegendLevel){
              return newLegendLevel.id === legendLevel.id;
            });
            if (!found){
              legendLevelPromises.push(self.removeLegendLevel({id: legendLevel.id}, options));
            }
          });
          return Promise.all(legendLevelPromises);
        })

        .then(function(){
          models.db.Legend.update(
            legendObject,
            Utils.extend({
              fields: ['name', 'description'],
              where: restriction
            }, options))

            .then(function(legend) {
              return resolve();
            })
        })

        .catch(function(err) {
          return reject(new Error("Could not update Legend " + err.toString()));
        });
    });
  },

  /**
   * It performs update legend level from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} alertObject - A legend level values to update
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   */
  updateLegendLevel: function(restriction, legendLevelObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      models.db.LegendLevel.update(
        legendLevelObject,
        Utils.extend({
          fields: ['name', 'value', 'level'],
          where: restriction
        }, options))
        .then(function(){
          return resolve();
        })
        .catch(function(err){
          return reject(new Error("Could not update LegendLevel " + err.toString()));
        })
    })
  },

  /**
   * It performs update alert notification from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} alertNotificationObject - An alert notification values to update
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   */
  updateAlertNotification: function(restriction, alertNotificationObject, options){
    var self = this;
    return new Promise(function(resolve, reject){
      models.db.AlertNotification.update(
        alertNotificationObject,
        Utils.extend({
          fields: ['include_report', 'notify_on_change', 'simplified_report', 'notify_on_legend_level', 'recipients'],
          where: restriction
        }, options))
        .then(function(){
          return resolve();
        })
        .catch(function(err){
          return reject(new Error("Could not update alert notification " + err.toString()));
        })
    })
  },

  /**
   * It removes an alert from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  removeAlert: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      var alert;
      var view;
      return self.getAlert(restriction, options)
        .then(function(alertResult) {
          alert = alertResult;
          view = alertResult.view;
          return self.removeAutomaticSchedule({id: alert.automatic_schedule.id}, options);
        })

        .then(function() {
          return models.db.Alert.destroy(Utils.extend({where: restriction}, options));
        })

        .then(function() {
          if (view.id)
            return self.removeView({id: view.id}, options);
          else
            return null;
        })
        .then(function(){
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not remove alert " + err.toString()));
        });
    });
  },

  /**
   * It removes legend level of database from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  removeLegendLevel: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.LegendLevel.destroy(Utils.extend({where: restriction}, options))
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(err);
        });
    });
  },

  /**
   * It removes alert notification of database from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  removeAlertNotification: function(restriction, options){
    var self = this;
    return new Promise(function(resolve, reject){
      return models.db.AlertNotification.destroy(Utils.extend({where: restriction}, options))
        .then(function(){
          return resolve();
        })
        .catch(function(err){
          return reject(err);
        });
    });
  },

  /**
   * It retrieves a list of views in database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.View[]>}
   */
  listViews: function(restriction, options) {
    var self = this;

    return new Promise(function(resolve, reject) {
      return models.db.View.findAll(Utils.extend({
        include: [
          {
            model: models.db.Schedule,
            required: false
          },
          {
            model: models.db.AutomaticSchedule,
            required: false
          },
          {
            model: models.db.DataSeries,
            include: [
              {
                model: models.db.DataProvider
              },
              {
                model: models.db.DataSeriesSemantics
              }
            ]
          },
          {
            model: models.db.ViewStyleLegend,
            required: false,
            include: [
              {
                model: models.db.ViewStyleColor
              },
              {
                model: models.db.ViewStyleLegendMetadata
              }
            ]
          },
          {
            model: models.db.ViewProperties,
            require: false
          }
        ],
        where: restriction
      }, options))
        .then(function(views) {
          return resolve(views.map(function(view) {
            var viewModel = new DataModel.View(Object.assign(view.get(), {
              schedule: view.Schedule ? new DataModel.Schedule(view.Schedule.get()) : {},
              automaticSchedule: view.AutomaticSchedule ? new DataModel.AutomaticSchedule(view.AutomaticSchedule.get()) : {},
              dataSeries: view.DataSery ? new DataModel.DataSeries(view.DataSery.get()) : {}
              // schedule: new DataModel.Schedule(view.Schedule ? view.Schedule.get() : {id: 0})
            }));
            if (view.ViewStyleLegend) {
              var legendModel = new DataModel.ViewStyleLegend(Utils.extend(
                view.ViewStyleLegend.get(), {colors: view.ViewStyleLegend.ViewStyleColors ? view.ViewStyleLegend.ViewStyleColors.map(function(elm) { return elm.get(); }) : []}));
              legendModel.setMetadata(Utils.formatMetadataFromDB(view.ViewStyleLegend.ViewStyleLegendMetadata));
              viewModel.setLegend(legendModel);
            }
            if (view.ViewProperties){
              viewModel.setProperties(view.ViewProperties);
            }
            return viewModel;
          }));
        })

        .catch(function(err) {
          return reject(new Error("Could not list views " + err.toString()));
        });
    });
  },
  /**
   * It performs a save view style color and retrieve it
   *
   * @param {Object} colorObject - View Style Color object to save
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<Object>} A view style object created
   */
  addViewStyleColor: function(colorObject, options) {
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleColor.create(colorObject, options)
        .then(function(colorResult) {
          return resolve(colorResult.get());
        })
        .catch(function(err) {
          return reject(new exceptions.ViewStyleColorError(Utils.format("Could not save view style color due %s", err.toString())));
        });
    });
  },
  /**
   * It performs a save view legend
   *
   * @param {Object} styleLegendObject         - A type object to save
   * @param {string} styleLegendObject.type_id - View Style Type identifier
   * @param {string} styleLegendObject.view_id - View identifier
   * @param {string} styleLegendObject.column  - Target column name
   * @param {any[]}  styleLegendObject.colors  - Color array to save
   * @param {Object} options                   - An ORM query options
   * @param {Transaction} options.transaction  - An ORM transaction
   * @returns {Promise<Object>}
   */
  addViewStyleLegend: function(styleLegendObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleLegend.create(styleLegendObject, options)
        .then(function(legendResult) {
          var promises = [];

          for(var i = 0; i < styleLegendObject.colors.length; ++i) {
            var color = styleLegendObject.colors[i];
            color.view_style_id = legendResult.id;
            promises.push(self.addViewStyleColor(color, options));
          }

          return Promise.all(promises)
            .then(function(colors) {
              return new DataModel.ViewStyleLegend(Utils.extend(legendResult.get(), {colors: colors}));
            });
        })
        // on success legend saving, prepare metadata
        .then(function(legendModel) {
          var metadataArr = Utils.generateArrayFromObject(styleLegendObject.metadata, function(key, value) {
            return {key: key, value: value, legend_id: legendModel.id};
          });
          return Promise.all([legendModel, self.addViewStyleLegendMetadata(metadataArr, options)]);
        })
        // on success legend metadata saving, set into model and resolve promise
        .spread(function(legendModel, legendMetadata) {
          legendModel.setMetadata(legendMetadata);

          return resolve(legendModel);
        })
        // any error
        .catch(function(err) {
          return reject(new Error(Utils.format("Could not save view style due %s", err.toString())));
        });
    });
  },
  addViewStyleLegendMetadata: function(metadataArr, options) {
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleLegendMetadata.bulkCreate(metadataArr, options)
        .then(function(metadataArrResults) {
          return resolve(Utils.formatMetadataFromDB(metadataArrResults));
        })
        .catch(function(err) {
          return reject(new Error(Utils.format("Could not save style legend metadata due %s", err.toString())));
        });
    });
  },
  listViewStyleLegendMetadata: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleLegendMetadata.findAll(Utils.extend({where: restriction}, options))
        .then(function(metadataResults) {
          return resolve(Utils.formatMetadataFromDB(metadataResults));
        })
        .catch(function(err) {
          return reject(err);
        });
    });
  },
  upsertViewStyleLegendMetadata: function(restriction, metadataObj, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return self.listViewStyleLegendMetadata(restriction, options)
        .then(function(metadataResults) {
          var lengthResult = Object.keys(metadataResults).length;
          if (lengthResult === 0) {
            // add
            return self.addViewStyleLegendMetadata([metadataObj], options);
          } else if (lengthResult === 1) {
            // update
            return models.db.ViewStyleLegendMetadata.update(metadataObj, Utils.extend({
                fields: ["key", "value"],
                where: restriction
              }, options));
          }
          throw new Error("More than one element retrieved during view legend metadata operation");
        })

        .then(function() {
          return resolve();
        })

        .catch(function(err) {
          return reject(err);
        });
    });
  },
  removeViewStyleLegendMetadata: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleLegendMetadata.destroy(Utils.extend({where: restriction}, options))
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(err);
        });
    });
  },
  /**
   * It performs update or insert view legend color in database.
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} styleColorObject         - A type object to save
   * @param {string} styleColorObject.title - Color title
   * @param {string} styleColorObject.color - Color value (hex)
   * @param {string} styleColorObject.view_legend_id  - Respective Legend identifier
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  upsertViewStyleColor: function(restriction, styleColorObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleColor.findOne(Utils.extend({where: restriction}, options))
        .then(function(colorResult) {
          if (colorResult) {
            // update
            return models.db.ViewStyleColor.update(styleColorObject, Utils.extend({
              fields: ["title", "color", "value"],
              where: {
                id: colorResult.id
              }
            }, options));
          }
          return self.addViewStyleColor(styleColorObject, options);
        })
        .catch(function(err) {
          return reject(new exceptions.ViewStyleColorError(Utils.format("Could not find view style color %s", err.toString())));
        });
    });
  },
  /**
   * It removes view color of database from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise}
   */
  removeViewStyleColor: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleColor.destroy(Utils.extend({where: restriction}, options))
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(err);
        });
    });
  },
  /**
   * It performs update view legend in database. Once updated, it does not retrieves the row affected in order to keep integrity.
   *
   * @param {Object} restriction - A query restriction
   * @param {ViewStyleLegend} styleLegendObject         - A type object to save
   * @param {string} styleLegendObject.type_id - View Style Type identifier
   * @param {string} styleLegendObject.view_id - View identifier
   * @param {string} styleLegendObject.column  - Target column name
   * @param {any[]}  styleLegendObject.colors  - Color array to save
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<DataModel.View>}
   */
  updateViewStyleLegend: function(restriction, styleLegendObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleLegend.update(styleLegendObject, Utils.extend({
          fields: ["type"],
          where: restriction
        }, options))
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(new Error(Utils.format("Could not update view legend due %s", err.toString())));
        });
    });
  },
  /**
   * It removes a view legend of database using given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<DataModel.View>}
   */
  removeViewStyleLegend: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.ViewStyleLegend.destroy(Utils.extend({where: restriction}, options))
        .then(function() {
          return resolve();
        })
        .catch(function(err) {
          return reject(new Error(Utils.format("Could not remove legend due %s", err.toString())));
        });
    });
  },
  /**
   * It performs a save view in database
   *
   * @param {Object} viewObject - A view object value to save
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @returns {Promise<DataModel.View>}
   */
  addView: function(viewObject, options) {
    var self = this;

    return new Promise(function(resolve, reject) {
      var view;
      return models.db.View.create(viewObject, options)
        .then(function(viewResult) {
          view = viewResult;
          if (!Utils.isEmpty(viewObject.legend) && !Utils.isEmpty(viewObject.legend.metadata)) {
            var legend = viewObject.legend;
            legend.view_id = view.id;
            return self.addViewStyleLegend(legend, options);
          }
          return null;
        })

        .then(function(legend) {
          var promises = [legend];
          if (view.schedule_id) {
            promises.push(self.getSchedule({id: view.schedule_id}, options));
          } else if (view.automatic_schedule_id){
            promises.push(self.getAutomaticSchedule({id: view.automatic_schedule_id}, options));
          }
          return Promise.all(promises);
        })

        .spread(function(legend, schedule) {
          var objectToAssign = {
            legend: legend
          };
          if (view.schedule_id){
            objectToAssign.schedule = schedule || {};
            objectToAssign.automatic_schedule = {};
          }
          else {
            objectToAssign.schedule = {};
            objectToAssign.automatic_schedule = schedule || {};
          }
          return new DataModel.View(Object.assign(view.get(), objectToAssign));
        })
        .then(function(viewModel){
          if (viewObject.properties){
            var properties = viewObject.properties;
            var propertiesList = [];

            if (properties instanceof Array) {
              propertiesList = properties;
            } else if (properties instanceof Object) {
              for(var key in properties) {
                if (properties.hasOwnProperty(key)) {
                  propertiesList.push({
                    view_id: view.id,
                    key: key,
                    value: properties[key]
                  });
                }
              }
            }
            return models.db.ViewProperties.bulkCreate(propertiesList, Utils.extend({view_id: view.id}, options)).then(function () {
              return models.db.ViewProperties.findAll(Utils.extend({where: {view_id: view.id}}, options)).then(function(viewProperties) {
                viewModel.setProperties(viewProperties);
                return resolve(viewModel);
              });
            });
          } else {
            return resolve(viewModel)
          }
        })

        .catch(function(err) {
          return reject(new Error("Could not create view " + err.toString()));
        });
    });
  },

  /**
   * It performs update views from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} viewObject - A view object values to update
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.View[]>}
   */
  updateView: function(restriction, viewObject, options) {
    var self = this;

    return new Promise(function(resolve, reject) {
      models.db.View.update(
        viewObject,
        Utils.extend({
          fields: ["name", "description", "data_series_id", "style", "active", "service_instance_id", "schedule_id", "automatic_schedule_id", "schedule_type", "private", "source_type"],
          where: restriction
        }, options))

        .then(function(view) {
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not update view " + err.toString()));
        });
    });
  },

  /**
   * It retrieves a view from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.View>}
   */
  getView: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      self.listViews(restriction, options)
        .then(function(views) {
          if (views.length === 0) {
            return reject(new Error("No view retrieved"));
          }

          if (views.length > 1) {
            return reject(new Error("Get operation retrieved more than a view"));
          }

          return resolve(views[0]);
        })
        .catch(function(err) {
          return reject(err);
        });
    });
  },
  /**
   * It removes a view from database
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise}
   */
  removeView: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      var view;
      return self.getView(restriction, options)
        .then(function(viewResult) {
          view = viewResult;
          if (view.schedule && view.schedule.id) {
            return self.removeSchedule({id: view.schedule.id}, options);
          } else if (view.automaticSchedule && view.automaticSchedule.id){
            return self.removeAutomaticSchedule({id: view.automaticSchedule.id}, options);
          } else {
            return null;
          }
        })

        .then(function() {
          return models.db.View.destroy(Utils.extend({where: restriction}, options));
        })

        .then(function() {
          return resolve();
        })

        .catch(function(err) {
          return reject(new Error("Could not remove view " + err.toString()));
        });
    });
  },
  /**
   * It saves a layer in database
   *
   * @param {number} registeredViewId - A TerraMA² Registered View Identifier
   * @param {Object} layerObject - TerraMA² Layer Object
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.RegisteredView>}
   */
  addLayer: function(registeredViewId, layerObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      layerObject.registered_view_id = registeredViewId;
      return models.db.Layer.create(layerObject, options)
        .then(function(layer) {
          return resolve(layer.get());
        })

        .catch(function(err) {
          return reject(new Error(Utils.format("Could not save layer due %s", err.toString())));
        });
    });
  },
  /**
   * It removes a view from database
   *
   * @param {Object} registeredViewObject - TerraMA² RegisteredView values
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.RegisteredView>}
   */
  addRegisteredView: function(registeredViewObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.RegisteredView.create(registeredViewObject, options)
        .then(function(viewResult) {
          var promises = registeredViewObject.layers_list.map(function(layer) {
            // TODO: Currently, layer is a object {layer: layerName}. It must be removed. Layer must be a string
            return self.addLayer(viewResult.id, {name: layer.layer}, options);
          });
          return Promise.all(promises)
            .then(function(layers) {
              return self.getView({id: viewResult.view_id})
                .then(function(view) {
                  return resolve(new DataModel.RegisteredView(Utils.extend(viewResult, {layers: layers, view: view})));
                });
            });
        })

        .catch(function(err) {
          return reject(new exceptions.RegisteredViewError(
            Utils.format("Coult not save Registered View due %s", err.toString())));
        });
    });
  },
  /**
   * It retrives a list of registered views in database from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.RegisteredView>}
   */
  listRegisteredViews: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      /**
       * It defines a list of model registered views
       * @type {Sequelize.Model[]}
       */
      var registeredViews = [];

      return models.db.RegisteredView.findAll(Utils.extend({
        where: restriction,
          // joins
          include: [
          {
            model: models.db.Layer
          },
          {
            model: models.db.View,
            include: [
              {
                model: models.db.DataSeries
              }
            ]
          }
        ]
      }, options))
        .then(function(registeredViewsResult) {
          registeredViews = registeredViewsResult;
          // retrieve all static data series
          return self.listDataSeries({
            data_series_semantics: {
              temporality: Enums.TemporalityType.STATIC
            }
          }, options);
        })
        // preparing data series to next handler
        .then(function(staticDataSeries) {
          return self.listDataSeries({Collector: {}}, options)
            .then(function(dynamicDataSeries) {
              return self.listDataSeries({Analysis: {}}, options)
                .then(function(analysisDataSeries) {
                  return {
                    static: staticDataSeries,
                    analysis: analysisDataSeries,
                    dynamic: dynamicDataSeries
                  };
                });
            });
        })
        // It will contains a object with data series: {dynamic: [...], static: [...], analysis: [...]}
        .then(function(cachedDataSeries) {
          /**
           * It defines a list of TerraMA² registered views to resolve
           * @type {RegisteredView[]}
           */
          var output = [];

          registeredViews.forEach(function(registeredView) {
            Object.keys(cachedDataSeries).some(function(key) {
              var dataSeriesList = cachedDataSeries[key];

              return dataSeriesList.some(function(dataSeries) {
                if (dataSeries.id === registeredView.View.data_series_id) {
                  var dModel = new DataModel.RegisteredView(registeredView.get());
                  dModel.setDataSeriesType(registeredView.View.source_type);
                  dModel.setDataSeries(dataSeries);
                  output.push(dModel);
                  return true;
                }
              });
            });
          });

          return resolve(output);
        })

        .catch(function(err) {
          return reject(new exceptions.RegisteredViewError(
            Utils.format("Could not retrieve registered views due %s", err.toString())));
        });
    });
  },
  /**
   * It retrives only a one registered view. If restriction applies for 0 results or more than 1, it throws Error
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.RegisteredView>}
   */
  getRegisteredView: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return self.listRegisteredViews(restriction, Utils.extend(options, {limit: 1}))
        .then(function(registeredViewList) {
          if (registeredViewList.length === 0) {
            return reject(new exceptions.RegisteredViewError("No registered views retrieved."));
          }
          if (registeredViewList.length > 1) {
            return reject(new exceptions.RegisteredViewError("More than one registered view retrieved in get operation"));
          }
          return resolve(registeredViewList[0]);
        });
    });
  },
  /**
   * It applies update over registered view.
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} registeredObject - TerraMA² registered object values to update
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.RegisteredView>}
   */
  updateRegisteredView: function(restriction, registeredObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return models.db.RegisteredView.update(registeredObject, Utils.extend({
        fields: ['workspace'],
        where: restriction
      }, options))
        .then(function() {
          return resolve();
        })

        .catch(function(err) {
          return reject(new exceptions.RegisteredViewError(
            Utils.format("Could not update registered view due %s", err.toString())));
        });
    });
  },
  /**
   * It performs an update or insert layer operation. When a layer found, it updates. Otherwise, a new layer will be created.
   *
   * @param {Object} restriction - A query restriction
   * @param {Object} layersObject - A Layer object
   * @param {string} layersObject.name - A Layer name
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<DataModel.RegisteredView>}
   */
  upsertLayer: function(restriction, layersObject, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      models.db.Layer.findOne(Utils.extend({
        where: restriction
      }, options))
        .then(function(layerResult) {
          // if retrieved a layer, tries update
          if (layerResult) {
            return models.db.Layer.update(layersObject, Utils.extend({
              fields: ['name'],
              where: {
                id: layerResult.id
              }
            }, options));
          } else {
            return self.addLayer(restriction.registered_view_id || layersObject.registered_view_id, layersObject, options);
          }
        })
        // on success insert|update
        .then(function() {
          return resolve();
        })

        .catch(function(err) {
          return reject(new exceptions.RegisteredViewError(
            Utils.format("Could not update or insert layer due %s", err.toString())));
        });
    });
  },
  /**
   * It retrieves all available script languages from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<any[]>}
   */
  listScriptLanguages: function(restriction, options) {
    return new Promise(function(resolve, reject) {
      return models.db.ScriptLanguage.findAll(Utils.extend({where: restriction}, options))
        .then(function(scriptLanguages) {
          return resolve(scriptLanguages.map(function(language) {
            return language.get();
          }));
        })
        .catch(function(err) {
          return reject(new Error(Utils.format("Could not list script languages %s", err.toString())));
        });
    });
  },
  /**
   * It gets a script language from given restriction
   *
   * @param {Object} restriction - A query restriction
   * @param {Object?} options - An ORM query options
   * @param {Transaction} options.transaction - An ORM transaction
   * @return {Promise<any>}
   */
  getScriptLanguage: function(restriction, options) {
    var self = this;
    return new Promise(function(resolve, reject) {
      return self.listScriptLanguages(restriction, Utils.extend(options, {limit: 1}))
        .then(function(scriptLanguageArray) {
          if (scriptLanguageArray.length === 0) {
            return reject(new Error("No script language found"));
          }
          return resolve(scriptLanguageArray[0]);
        })

        .catch(function(err) {
          return reject(err);
        });
    });
  }
};
