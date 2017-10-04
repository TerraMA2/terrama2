"use strict";

var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var ProjectError = require("../../core/Exceptions").ProjectError;
var TokenCode = require('./../../core/Enums').TokenCode;
var Promise = require('bluebird');
var TcpService = require('./../../core/facade/tcp-manager/TcpService');
var fs = require('fs');
var path = require("path");
var Application = require("./../../core/Application");


module.exports = function(app) {
  return {
    post: function(request, response) {
      var projectObject = request.body;
      projectObject.user_id = request.user.id;

      DataManager.addProject(projectObject).then(function(project) {
        TcpService.emitEvent("projectReceived", project);

        // Creating default PostGIS and File providers
        var configFile = Application.getContextConfig();

        // File data provider object
        var DefaultFileProvider = {
          name: "Local Folder",
          uri: "file://" + configFile.defaultFilePath,
          description: "Local Folder data server",
          data_provider_intent_id: 1,
          data_provider_type_id: 1,
          project_id: project.id,
          active: true
        };

        // PostGIS data provider object
        var uriPostgis = "postgis://" + configFile.db.username + ":" + configFile.db.password + "@" + configFile.db.host + ":5432/" + configFile.db.database;
        var DefaultPostgisProvider = {
          name: "Local Database PostGIS",
          uri: uriPostgis,
          description: "Local Database PostGIS data server",
          data_provider_intent_id: 1,
          data_provider_type_id: 4,
          project_id: project.id,
          active: true
        };
        var promises = [];
        promises.push(DataManager.addDataProvider(DefaultFileProvider));
        promises.push(DataManager.addDataProvider(DefaultPostgisProvider));

        return Promise.all(promises).then(function(providers){
          //sending providers to service
          providers.forEach(function(provider){
            TcpService.send({
              "DataProviders": [provider.toService()]
            });
          });
          var token = Utils.generateToken(app, TokenCode.SAVE, project.name);
          response.json({status: 200, result: project, token: token});
        });
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      });
    },

    get: function(request, response) {
      var id = request.params.id;

      if (id) {
        DataManager.getProject({id: id}).then(function(project) {
          response.json(project);
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        });
      } else {
        var projects = DataManager.listProjects();
        projects.forEach(function(project){
          project.hasPermission = !project.protected || request.user.administrator || request.user.id == project.user_id;
        });
        response.json(projects);
      }
    },

    put: function(request, response) {
      var id = request.params.id;

      if (id) {
        var projectGiven = request.body;
        projectGiven.id = id;
        projectGiven.user_id = projectGiven.user_id ? projectGiven.user_id : request.user.id;
        DataManager.updateProject(projectGiven).then(function(project) {
          TcpService.emitEvent("projectReceived", project);

          var token = Utils.generateToken(app, TokenCode.UPDATE, project.name);
          response.json({status: 200, result: project, token: token});
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });
      } else {
        Utils.handleRequestError(response, new ProjectError("Project name not identified"), 400);
      }
    },

    delete: function(request, response) {
      var id = request.params.id;
      if (id) {
        DataManager.getProject({id: id}).then(function(project) {
          var objectToSend = {
            "Alerts": [],
            "Analysis": [],
            "Collectors": [],
            "DataProvider": [],
            "DataSeries": [],
            "Legends": [],
            "Views": []
          };

          var dataProviders = DataManager.listDataProviders({ project_id: id });

          dataProviders.forEach(function(dataProvider) {
            objectToSend.DataProvider.push(dataProvider.id);
          });

          var collectorPromises = [];

          DataManager.listAlerts({ project_id: id }).then(function(alerts) {
            alerts.forEach(function(alert) {
              objectToSend.Alerts.push(alert.id);
            });

            return DataManager.listAnalysis({ project_id: id });
          }).then(function(analysisList) {
            analysisList.forEach(function(analysis) {
              objectToSend.Analysis.push(analysis.id);
            });

            return DataManager.listDataSeries({ project_id: id });
          }).then(function(dataSeriesList) {
            dataSeriesList.forEach(function(dataSeries) {
              objectToSend.DataSeries.push(dataSeries.id);

              collectorPromises.push(
                DataManager.getCollector({ data_series_output: dataSeries.id }).then(function(collector) {
                  objectToSend.Collectors.push(collector.id);
                })
              );
            });

            return DataManager.listLegends({ project_id: id });
          }).then(function(legends) {
            legends.forEach(function(legend) {
              objectToSend.Legends.push(legend.id);
            });

            return DataManager.listViews({ project_id: id });
          }).then(function(views) {
            views.forEach(function(view) {
              objectToSend.Views.push(view.id);
            });

            return Promise.all(collectorPromises).catch(function(err) {
              if(!err.name || (err.name && err.name != "CollectorErrorNotFound"))
                Utils.handleRequestError(response, new ProjectError("Failed to load dependents"), 400);
            });
          }).then(function() {
            DataManager.removeProject({id: id}).then(function() {
              TcpService.emitEvent("projectDeleted", { id: id });

              if(objectToSend.Alerts.length === 0)
                delete objectToSend.Alerts;

              if(objectToSend.Analysis.length === 0)
                delete objectToSend.Analysis;

              if(objectToSend.Collectors.length === 0)
                delete objectToSend.Collectors;

              if(objectToSend.DataProvider.length === 0)
                delete objectToSend.DataProvider;

              if(objectToSend.DataSeries.length === 0)
                delete objectToSend.DataSeries;

              if(objectToSend.Legends.length === 0)
                delete objectToSend.Legends;

              if(objectToSend.Views.length === 0)
                delete objectToSend.Views;

              if(
                objectToSend.hasOwnProperty("Alerts") ||
                objectToSend.hasOwnProperty("Analysis") ||
                objectToSend.hasOwnProperty("Collectors") ||
                objectToSend.hasOwnProperty("DataProvider") ||
                objectToSend.hasOwnProperty("DataSeries") ||
                objectToSend.hasOwnProperty("Legends") ||
                objectToSend.hasOwnProperty("Views")
              ) {
                TcpService.remove(objectToSend);
              }

              // un-setting cache project
              request.session.activeProject = {};

              var token = Utils.generateToken(app, TokenCode.DELETE, project.name);
              response.json({status: 200, name: project.name, token: token});
            }).catch(function(err) {
              Utils.handleRequestError(response, err, 400);
            });
          }).catch(function(err) {
            Utils.handleRequestError(response, new ProjectError("Failed to load dependents"), 400);
          });
        }).catch(function(err) {
          Utils.handleRequestError(response, new ProjectError("Project not found"), 400);
        });
      } else {
        Utils.handleRequestError(response, new ProjectError("Project id not typed"), 400);
      }
    }
  };
};
